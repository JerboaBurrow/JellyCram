#include "main.h"

const bool develop = true;

int main(int argc, char ** argv)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    conf.COCOA_RETINA = false;

    jGL::DesktopDisplay display(glm::ivec2(resX,resY),"Jelly Cram", conf);
    icon(display);

    glewInit();

    jGLInstance = std::move(std::make_shared<jGL::GL::OpenGLInstance>(display.getRes()));

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(1);

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    EntityComponentSystem manager;

    jLog::Log log;

    Hop::Console console(log);

    std::unique_ptr<AbstractWorld> world;

    float posX = 0.0;
    float posY = 0.0;

    Hop::World::FiniteBoundary mapBounds(0,0,16,16,true,false,true,true);
    Hop::World::FixedSource mapSource;

    world = std::make_unique<TileWorld>
    (
        2,
        &camera,
        16,
        0,
        &mapSource,
        &mapBounds  
    );
    
    sRender & rendering = manager.getSystem<sRender>();
    rendering.setDrawMeshes(true);

    // setup physics system
    sPhysics & physics = manager.getSystem<sPhysics>();
    physics.setTimeStep(deltaPhysics);
    physics.setGravity(gravity, 0.0, -1.0);

    sCollision & collisions = manager.getSystem<sCollision>();

    // a primitive is 1.0/(3*9) in radius
    // cell length <~ r is efficient
    auto det = std::make_unique<Hop::System::Physics::CellList>
    (
        27,
        Hop::Util::tupled(0.0,1.0),
        Hop::Util::tupled(0.0,1.0)
    );

    auto res = std::make_unique<Hop::System::Physics::SpringDashpot>
    (
        deltaPhysics*10.0,
        0.5,
        0.0
    );

    collisions.setDetector(std::move(det));
    collisions.setResolver(std::move(res));

    Hop::LuaExtraSpace luaStore;

    luaStore.ecs = &manager;
    luaStore.world = world.get();
    luaStore.physics = &physics;
    luaStore.resolver = &collisions;

    console.luaStore(&luaStore);
    console.runFile("config.lua");
    std::string status = console.luaStatus();
    if (status != "LUA_OK") { WARN(status) >> log; }

    high_resolution_clock::time_point t0, t1, tp0, tp1, tr0, tr1;

    double fx, fy, omega;

    Id current;

    manager.hasComponent<cCollideable>(current);

    std::vector<Id> objects;

    bool allowMove = true;
    bool incoming = false;
    bool begin = true;
    bool gameOver = false;

    uint64_t score = 0;
    uint64_t graceFrames = 60;

    double countDownSeconds = 5.0;
    double countDownDecrement = 0.1;
    double elapsed_countdown = 0.0;
    high_resolution_clock::time_point countDownBegin;

    double impulseSoftening = 0.975;
    double torqueSoftening = 0.975;
    double currentTorque = torque;
    double currentImpulse = impulse;

    console.runString("previewIndex = math.random(#meshes)");

    while (display.isOpen())
    {
        if (display.keyHasEvent(GLFW_KEY_F2, jGL::EventType::PRESS))
        {
            debug = !debug;
        }

        if (!gameOver)
        {

            fx = 0.0;
            fy = 0.0;
            omega = 0.0;

            if (!paused)
            {
                console.runFile("loop.lua");
            }

            Id id = manager.idFromHandle("current");

            if (begin)
            {
                current = id;
                objects.push_back(current);
            }

            if (!begin && id != current)
            {
                objects.push_back(id);
                allowMove = true;
                current = id;
                score += 1; // all tetrominoes have the same number of 3x3 blocks
                console.runString("previewIndex = math.random(#meshes)");
                countDownSeconds = std::max(countDownSeconds-countDownDecrement, minCountdown);
                currentImpulse = std::max(impulseSoftening*currentImpulse, minImpulse);
                currentTorque = std::max(torqueSoftening*currentTorque, minTorque);
            }

            if (collisions.objectHasCollided(current) != CollisionDetector::CollisionType::NONE)
            {
                if (collisions.objectHasCollided(current) == CollisionDetector::CollisionType::OBJECT)
                {
                    if (current != Id(-1) && manager.hasComponent<cCollideable>(current))
                    {
                        const cCollideable & c = manager.getComponent<cCollideable>(current);
                        if (objectOverTop(c, 1.0))
                        {
                            if (graceFrames <= 1)
                            {
                                gameOver = true;
                                fadeAll(objects,manager,0.33);
                            }
                            else
                            {
                                graceFrames -= 1;
                            }
                        }
                    }
                }

                if (allowMove) 
                { 
                    cRenderable & r = manager.getComponent<cRenderable>(current);
                    // fade outslightly
                    r.a = 0.75;
                    incoming = true; 
                    countDownBegin = high_resolution_clock::now();
                }
                allowMove = false;
            }

            if (display.getEvent(GLFW_KEY_SPACE).type == jGL::EventType::PRESS) 
            { 
                if (paused)
                {
                    if (!develop){fadeAll(objects,manager,1.0);}
                    countDownBegin = high_resolution_clock::now();
                }
                else
                {
                    if (!develop){fadeAll(objects,manager,0.0);}
                }
                paused = !paused; 
            }

            if (display.keyHasEvent(GLFW_KEY_W, jGL::EventType::PRESS))
            {
                fy += currentImpulse;
            }

            if (display.keyHasEvent(GLFW_KEY_S, jGL::EventType::PRESS))
            {
                fy -= currentImpulse;
            }

            if (display.keyHasEvent(GLFW_KEY_A, jGL::EventType::PRESS))
            {
                fx -= currentImpulse;
            }

            if (display.keyHasEvent(GLFW_KEY_D, jGL::EventType::PRESS))
            {
                fx += currentImpulse;
            }

            if (display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::PRESS))
            {
                omega -= currentTorque;
            }

            if (display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::PRESS))
            {
                omega += currentTorque;
            }

            if (allowMove && (fx != 0.0 || fy != 0.0))
            {
                physics.applyForce
                (
                    &manager,
                    id,
                    fx,
                    fy,
                    true
                );
            }

            if (allowMove && omega != 0.0)
            {
                physics.applyTorque
                (
                    &manager,
                    id,
                    omega
                );
            }
        }
        else
        {
            if (display.getEvent(GLFW_KEY_SPACE).type == jGL::EventType::PRESS) 
            { 
                for (auto o : objects)
                {
                    manager.remove(o);
                }
                objects.clear();
                
                allowMove = true;
                incoming = false;
                begin = true;
                gameOver = false;
                paused = false;

                score = 0;
                graceFrames = 60;

                countDownSeconds = 5;
                elapsed_countdown = 0.0;
                console.runString("previewIndex = math.random(#meshes)");

                currentImpulse = impulse;
                currentTorque = torque;
            }
        }

        jGLInstance->beginFrame();

            jGLInstance->clear();

            t0 = high_resolution_clock::now();

            world->updateRegion(posX,posY);

            glClearColor(1.0f,1.0f,1.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            tp0 = high_resolution_clock::now();

            collisions.centreOn(world.get()->getMapCenter());
            
            if (!paused)
            {
                physics.step(&manager, &collisions, world.get());
            }

            tp1 = high_resolution_clock::now();

            tr0 = high_resolution_clock::now();

            rendering.setProjection(camera.getVP());
            rendering.draw(jGLInstance, &manager, world.get()); 

            tr1 = high_resolution_clock::now();

            if (!gameOver && incoming && !paused)
            {
                elapsed_countdown += duration_cast<duration<double>>(high_resolution_clock::now()-countDownBegin).count();
                countDownBegin = high_resolution_clock::now();

                if (elapsed_countdown >= countDownSeconds)
                {
                    incoming = false;
                    console.runString("nextPiece = true");
                    elapsed_countdown = 0.0;
                }
                else
                {
                    double t = countDownSeconds-elapsed_countdown;
                    t = std::floor(t * 100.0)/100.0;
                    jGLInstance->text
                    (
                        fixedLengthNumber(t, 4),
                        glm::vec2(resX*0.5f,resY-96.0f),
                        0.3f*t,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(true,false)
                    );
                }
            }
            
            if (gameOver)
            {
                jGLInstance->text
                (
                    "Game Over\nScore: "+std::to_string(int(score))+"\nSpace to replay",
                    glm::vec2(resX*0.5f,resY-64.0f),
                    0.5f,
                    glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                    glm::bvec2(true,false)
                );
            }
            else
            {
                jGLInstance->text
                (
                    "Score: "+std::to_string(int(score)),
                    glm::vec2(resX*0.5f,resY-32.0f),
                    0.5f,
                    glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                    glm::bvec2(true,false)
                );
            }

            if (debug)
            {
                double delta = 0.0;
                for (int n = 0; n < 60; n++)
                {
                    delta += deltas[n];
                }
                delta /= 60.0;
                std::stringstream debugText;

                double pdt = duration_cast<duration<double>>(tp1 - tp0).count();
                double rdt = duration_cast<duration<double>>(tr1 - tr0).count();

                double mouseX, mouseY;
                display.mousePosition(mouseX,mouseY);

                float cameraX = camera.getPosition().x;
                float cameraY = camera.getPosition().y;

                glm::vec4 worldPos = camera.screenToWorld(mouseX,mouseY);

                Hop::World::TileData tile = world->getTileData(worldPos[0],worldPos[1]);

                debugText << "Delta: " << fixedLengthNumber(delta,6) <<
                    " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
                    "\n" <<
                    "Mouse (" << fixedLengthNumber(mouseX,4) << "," << fixedLengthNumber(mouseY,4) << ")" <<
                    "\n" <<
                    "Mouse [world] (" << fixedLengthNumber(worldPos[0],4) << "," << fixedLengthNumber(worldPos[1],4) << ")" <<
                    "\n" <<
                    "Mouse cell (" << fixedLengthNumber(tile.x,4) << ", " << fixedLengthNumber(tile.y,4) << ", " << tile.tileType <<
                    "\n" <<
                    "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
                    "\n" << 
                    "update time: " << fixedLengthNumber(pdt+rdt,6) <<
                    "\n" <<
                    "Phys update / draw time: " << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6) <<
                    "\n" <<
                    "Kinetic Energy: " << fixedLengthNumber(physics.kineticEnergy(),6);

                jGLInstance->text
                (
                    debugText.str(),
                    glm::vec2(64.0f,resY-64.0f),
                    0.5f,
                    glm::vec4(0.0f,0.0f,0.0f, 1.0f)
                );

            }

            if (frameId == 30)
            {
                if (log.size() > 0)
                {
                    std::cout << log.get() << "\n";
                }
            }

        jGLInstance->endFrame();

        display.loop();

        t1 = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(t1 - t0).count();
        frameId = (frameId+1) % 60;

        if (frameId == 0)
        {
            checkDelete(objects, manager, 3.0/(3*9), 9);
        }
        
        begin = false;

    }

    jGLInstance->finish();

    // force some static strings to appear in the binary, as a message to anyone using strings or hexdump
    message();

    return 0;
}
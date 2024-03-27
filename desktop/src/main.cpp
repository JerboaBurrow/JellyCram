#include <gameState.h>
#include <desktop.h>
#include <setup_lua.h>
#include <loop_lua.h>

void run_lua_loop(Hop::Console & console, std::string script)
{
    if (script == "loop.lua")
    {
        console.runString(loop_lua);
    }
}


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

    std::shared_ptr<jGL::Texture> menuIcon;
    std::shared_ptr<jGL::Texture> dismissIcon;
    std::shared_ptr<jGL::SpriteRenderer> sprites;

    try
    {
        menuIcon = jGLInstance->createTexture
        (
            "res/menu.png",
            jGL::Texture::Type::RGBA
        );

        dismissIcon = jGLInstance->createTexture
        (
            "res/menu-dismiss.png",
            jGL::Texture::Type::RGBA
        );

        sprites = jGLInstance->createSpriteRenderer
        (
            2
        );

        sprites->setProjection(camera.getVP());

        sprites->add
        (
            {
                jGL::Transform(menuX, menuY, 0.0f, menuScale),
                jGL::TextureOffset(0.0f, 0.0f),
                menuIcon
            },
            "menuIcon"
        );

        sprites->add
        (
            {
                jGL::Transform(menuX, menuY, 0.0f, menuScale),
                jGL::TextureOffset(0.0f, 0.0f),
                dismissIcon
            },
            "dismissIcon"
        );

        INFO("Loaded icons") >> log;
        loadedIcons = true;
    }
    catch (std::runtime_error e)
    {
        ERROR("Could no load icons") >> log;
    }

    Hop::World::FiniteBoundary<double> mapBounds(0,0,16,16,true,false,true,true);
    Hop::World::FixedSource mapSource;
    
    std::shared_ptr<AbstractWorld> world = std::make_shared<TileWorld>
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

    collisions.centreOn(world.get()->getMapCenter());

    Hop::LuaExtraSpace luaStore;

    luaStore.ecs = &manager;
    luaStore.world = world.get();
    luaStore.physics = &physics;
    luaStore.resolver = &collisions;

    console.luaStore(&luaStore);
    std::string status = console.luaStatus();
    if (status != "LUA_OK") { WARN(status) >> log; }

    high_resolution_clock::time_point t0, t1, tp0, tp1, tr0, tr1;

    bool begin = true;

    console.runString(setup_lua);
    console.runString("previewIndex = math.random(#meshes-1)");
    console.runString("nextX = 0.5;");
    double r = 3.0/27.0;

    JellyCramState state;

    state.lengthScale = r;

    Controls bindings;

    while (display.isOpen())
    {

        if (display.keyHasEvent(GLFW_KEY_F2, jGL::EventType::PRESS))
        {
            state.debug = !state.debug;
        }

        if (display.keyHasEvent(GLFW_MOUSE_BUTTON_1, jGL::EventType::PRESS))
        {
            double mouseX, mouseY;
            display.mousePosition(mouseX,mouseY);
            glm::vec4 worldPos = camera.screenToWorld(mouseX,mouseY);

            double d2 = (menuX-worldPos.x)*(menuX-worldPos.x)+(menuY-worldPos.y)*(menuY-worldPos.y);
            if (d2 < menuScale*menuScale && bindings.ok())
            {
                if (!displayingMenu || (displayingMenu && bindings.ok()))
                {
                    displayingMenu = !displayingMenu;
                    state.events[Event::PAUSE] = true;
                    bindings.save();
                }
            }
        }

        if (!displayingMenu)
        {
            if (display.getEvent(bindings.get("Pause/unpause")).type == jGL::EventType::PRESS) 
            { 
                state.events[Event::PAUSE] = true;
                if (state.gameOver)
                {
                    begin = true;
                    state.restart(manager, console);
                }
            }

            if (display.keyHasEvent(bindings.get("Up"), jGL::EventType::PRESS))
            {
                state.events[Event::UP] = true;
            }

            if (display.keyHasEvent(bindings.get("Down"), jGL::EventType::PRESS))
            {
                state.events[Event::DOWN] = true;
            }

            if (display.keyHasEvent(bindings.get("Left"), jGL::EventType::PRESS))
            {
                state.events[Event::LEFT] = true;
            }

            if (display.keyHasEvent(bindings.get("Right"), jGL::EventType::PRESS))
            {
                state.events[Event::RIGHT] = true;
            }

            if (display.keyHasEvent(bindings.get("Left rotate"), jGL::EventType::PRESS))
            {
                state.events[Event::ROT_LEFT] = true;
            }

            if (display.keyHasEvent(bindings.get("Right rotate"), jGL::EventType::PRESS))
            {
                state.events[Event::ROT_RIGHT] = true;
            }
        }
        else
        {
            if (display.keyHasEvent(GLFW_MOUSE_BUTTON_1, jGL::EventType::PRESS))
            {
                double mouseX, mouseY;
                display.mousePosition(mouseX,mouseY);
                mouseY = resY-mouseY;
                int hash = (mouseY-keySelectHeight-keySelectY)/(1.5*keySelectHeight);
                switch (hash)
                {
                    case 0:
                        selectingKey = "Up";
                        break;
                    case -1:
                        selectingKey = "Down";
                        break;
                    case -2:
                        selectingKey = "Left";
                        break;
                    case -3:
                        selectingKey = "Right";
                        break;
                    case -4:
                        selectingKey = "Left rotate";
                        break;
                    case -5:
                        selectingKey = "Right rotate";
                        break;
                    case -6:
                        selectingKey = "Pause/unpause";
                        break;
                    default:
                        selectingKey = "";
                        break;
                }
            }
            else if (std::find(controls.cbegin(), controls.cend(), selectingKey) != controls.cend())
            {
                for (auto code : keyCodes)
                {
                    if 
                    (
                        code.first != GLFW_KEY_ESCAPE && 
                        code.first != GLFW_KEY_F2     &&
                        display.keyHasEvent(code.first, jGL::EventType::PRESS)
                    )
                    {
                        bindings.set(selectingKey, code.first);
                    }
                }
            }
        }

        tp0 = high_resolution_clock::now();

        state.iteration
        (
            manager,
            console,
            collisions,
            physics,
            world,
            &run_lua_loop,
            frameId,
            begin
        );

        tp1 = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->clear();

            t0 = high_resolution_clock::now();

            glClearColor(1.0f,1.0f,1.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            tr0 = high_resolution_clock::now();

            rendering.setProjection(camera.getVP());
            rendering.draw(jGLInstance, &manager, world.get()); 

            tr1 = high_resolution_clock::now();

            if (!state.gameOver && state.incoming && !state.paused)
            {
                double t = state.countDownSeconds-state.elapsed_countdown;
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
            
            if (state.gameOver)
            {
                jGLInstance->text
                (
                    "Game Over\nScore: "+std::to_string(int(state.score))+"\nSpace to replay",
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
                    "Score: "+std::to_string(int(state.score)),
                    glm::vec2(resX*0.5f,resY-32.0f),
                    0.5f,
                    glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                    glm::bvec2(true,false)
                );
            }

            if (state.debug)
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

                glm::vec4 worldPos = camera.screenToWorld(mouseX,mouseY);

                Hop::World::TileData tile = world->getTileData(worldPos[0],worldPos[1]);

                double unsettlement = energy(state.objects, manager) / (1.0+state.objects.size());
                double threshold = state.currentSettleThreshold*state.currentSettleThreshold;

                debugText << "Delta: " << fixedLengthNumber(delta,6) <<
                    " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
                    "\n" <<
                    "Mouse (" << fixedLengthNumber(mouseX,4) << "," << fixedLengthNumber(mouseY,4) << ")" <<
                    "\n" <<
                    "Mouse [world] (" << fixedLengthNumber(worldPos[0],4) << "," << fixedLengthNumber(worldPos[1],4) << ")" <<
                    "\n" <<
                    "Mouse cell (" << fixedLengthNumber(tile.x,4) << ", " << fixedLengthNumber(tile.y,4) << ", " << tile.tileType <<
                    "\n" <<
                    "Unsettlement / threshold: " << fixedLengthNumber(unsettlement,6) << " / " << fixedLengthNumber(threshold,6) <<
                    "\n" << 
                    "Impulse (g) / Torque / countDown (s): " << fixedLengthNumber(state.currentImpulse/gravity,4) << " / " << fixedLengthNumber(state.currentTorque,4) << " / " << fixedLengthNumber(state.countDownSeconds,4) <<
                    "\n" << 
                    "Smasher prob: " << fixedLengthNumber(state.currentSmasherProb, 4) <<
                    "\nupdate time: " << fixedLengthNumber(pdt+rdt,6) <<
                    "\n" <<
                    "state update / draw time: " << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6) <<
                    "\n" <<
                    "Kinetic Energy: " << fixedLengthNumber(physics.kineticEnergy(),6) <<
                    "\nThis is debug output, press F2 to dismiss";

                jGLInstance->text
                (
                    debugText.str(),
                    glm::vec2(64.0f,resY-64.0f),
                    0.5f,
                    glm::vec4(0.0f,0.0f,0.0f, 1.0f)
                );

            }

            if (displayingMenu)
            {
                if (loadedIcons)
                {
                    sprites->getSprite("dismissIcon").update(jGL::Transform(menuX, menuY, 0.0f, menuScale));
                    sprites->getSprite("menuIcon").update(jGL::Transform(-menuX, menuY, 0.0f, menuScale));
                }
                else
                {
                    jGLInstance->text
                    (
                        "X",
                        glm::vec2(menuX*resX,menuY*resY),
                        0.5f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(true,false)
                    );
                }
                double x = keySelectX;
                double y = keySelectY;

                if (selectingKey != "")
                {
                    jGLInstance->text
                    (
                        "Press a key to bind: "+selectingKey,
                        glm::vec2(x,y+keySelectHeight*1.5),
                        0.5f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(false,false)
                    );
                }
                else
                {
                    jGLInstance->text
                    (
                        "Click a control to rebind",
                        glm::vec2(x,y+keySelectHeight*1.5),
                        0.5f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(false,false)
                    );
                }

                for (std::string key : controls)
                {
                    int binding = bindings.get(key);
                    jGLInstance->text
                    (
                        key,
                        glm::vec2(x,y),
                        0.5f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(false,false)
                    );

                    jGLInstance->text
                    (
                        keyCodes.at(binding),
                        glm::vec2(x+keySelectXGap,y),
                        0.5f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(false,false)
                    );

                    y -= keySelectHeight*1.5;
                }            
            }
            else
            {
                if (loadedIcons)
                {
                    sprites->getSprite("menuIcon").update(jGL::Transform(menuX, menuY, 0.0f, menuScale));
                    sprites->getSprite("dismissIcon").update(jGL::Transform(-menuX, menuY, 0.0f, menuScale));
                }
                else
                {
                    jGLInstance->text
                    (
                        "menu",
                        glm::vec2(menuX*resX,menuY*resY),
                        0.5f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(true,false)
                    );
                }
            }

            if (loadedIcons)
            {
                sprites->draw();
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
        
        begin = false;
        
    }

    jGLInstance->finish();

    // force some static strings to appear in the binary, as a message to anyone using strings or hexdump
    message();

    return 0;
}
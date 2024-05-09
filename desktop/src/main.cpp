#include <gameState.h>
#include <desktop.h>
#include <setup_lua.h>
#include <loop_lua.h>
#include <desktop_setup_lua.h>

#ifdef WINDOWS
#include <windows.h>
#include <mmsystem.h>
#endif

void run_lua_loop(Hop::Console & console, std::string script)
{
    if (script == "loop.lua")
    {
        console.runString(loop_lua);
    }
}

int main(int argc, char ** argv)
{
    #ifdef WINDOWS
        timeBeginPeriod(1);
    #endif
    glfwInit();

    // hack to obtain decoration size
    GLFWwindow * temporaryWindow = glfwCreateWindow(1, 1, "", NULL, NULL);
    int fleft, ftop, fright, fbottom;
    glfwGetWindowFrameSize(temporaryWindow, &fleft, &ftop, &fright, &fbottom);
    glfwWindowShouldClose(temporaryWindow);
    glfwDestroyWindow(temporaryWindow);

    // truncate to monitor
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    resX = std::min(resX, mode->width);
    resY = std::min(resY, mode->height);

    // get work area (i.e. without taskbars)
    int wxpos, wypos, wwidth, wheight;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &wxpos, &wypos, &wwidth, &wheight);

    if (resY+ftop > wheight)
    {
        resY = wheight-ftop;
    }

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    #ifdef APPLE
    // we get aa for free but does not play nice with MSAA
    conf.COCOA_RETINA = true;
    #else
    conf.COCOA_RETINA = false;
    #endif

    jGL::DesktopDisplay display(glm::ivec2(resX,resY),"Jelly Cram", conf);

    icon(display);

    glewInit();

    jGLInstance = std::move(std::make_shared<jGL::GL::OpenGLInstance>(display.getRes()));

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    #ifndef APPLE
        jGLInstance->setMSAA(16);
    #else
        jGLInstance->setMSAA(0);
    #endif

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    EntityComponentSystem manager;

    jLog::Log log;

    Hop::Console console(log);

    std::shared_ptr<jGL::Texture> menuIcon;
    std::shared_ptr<jGL::Texture> dismissIcon;
    std::shared_ptr<jGL::Texture> darkIcon;
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

        darkIcon = jGLInstance->createTexture
        (
            "res/dark.png",
            jGL::Texture::Type::RGBA
        );

        sprites = jGLInstance->createSpriteRenderer
        (
            3
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

        sprites->add
        (
            {
                jGL::Transform(menuX, menuY, 0.0f, menuScale),
                jGL::TextureOffset(0.0f, 0.0f),
                darkIcon
            },
            "darkIcon"
        );

        INFO("Loaded icons") >> log;
        loadedIcons = true;
    }
    catch (std::runtime_error e)
    {
        INFO("Could no load icons") >> log;
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

    high_resolution_clock::time_point tp0, tp1, tr0, tr1;

    bool begin = true;

    console.runString(setup_lua);
    console.runString(desktop_setup_lua);
    console.runString("previewIndex = math.random(#meshes-1)");
    console.runString("lastPreviewIndex = -1");
    console.runString("nextPiece = true");
    console.runString("nextX = 0.5;");
    if (darkMode) 
    {
        console.runString("previewColour = previewColourDarkMode");
    } else
    {
        console.runString("previewColour = previewColourLightMode");
    }
    double r = 3.0/27.0;

    JellyCramState state;

    state.lengthScale = r;

    Settings settings;
    
    bool savedTutorial = false;
    unsigned waited;

    std::chrono::high_resolution_clock::time_point frame_clock = std::chrono::high_resolution_clock::now();

    while (display.isOpen())
    {
        std::chrono::microseconds elapsed_micros = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-frame_clock);
        frame_clock = std::chrono::high_resolution_clock::now();
        waited = 0;
        if (elapsed_micros < std::chrono::microseconds(16666))
        {
            std::chrono::milliseconds wait(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::microseconds(16666)-elapsed_micros));
            if (wait.count() > 0)
            {
                #ifdef WINDOWS
                    SleepEx(DWORD(wait.count()), false);
                #else
                    std::this_thread::sleep_for(wait);
                #endif
                waited = wait.count();
            }
        }

        if (!savedTutorial && settings.tutorial.isDone())
        {
            savedTutorial = true;
            settings.save();
        }

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
            double ddark2 = (menuX-worldPos.x)*(menuX-worldPos.x)+(darkY-worldPos.y)*(darkY-worldPos.y);
            if (d2 < menuScale*menuScale && settings.ok())
            {
                if (!displayingMenu || (displayingMenu && settings.ok()))
                {
                    displayingMenu = !displayingMenu;
                    if ((displayingMenu && !state.paused) || !displayingMenu)
                    {
                        state.events[Event::PAUSE] = true;
                    }
                    settings.save();
                }
            }
            else if (ddark2 < menuScale*menuScale)
            {
                darkMode = !darkMode;
                double r, g, b;
                if (darkMode) 
                {
                    r = 1.0;
                    g = 1.0;
                    b = 1.0;
                    console.runString("previewColour = previewColourDarkMode");
                } else 
                {
                    r = 72.0/255.0;
                    g = r;
                    b = r;
                    console.runString("previewColour = previewColourLightMode");
                }

                if (manager.handleExists("preview")) 
                {
                    Id p = manager.idFromHandle("preview");
                    cRenderable & c = manager.getComponent<cRenderable>(p);
                    c.r = r;
                    c.g = g;
                    c.b = b;
                }
            }
        }

        if (!displayingMenu)
        {
            if (display.getEvent(settings.get("Pause/unpause")).type == jGL::EventType::PRESS) 
            { 
                state.events[Event::PAUSE] = true;
                if (state.gameOver)
                {
                    begin = true;
                    state.restart(manager, console);
                }
            }

            if (display.keyHasEvent(settings.get("Up"), jGL::EventType::PRESS))
            {
                state.events[Event::UP] = true;
            }

            if (display.keyHasEvent(settings.get("Down"), jGL::EventType::PRESS))
            {
                state.events[Event::DOWN] = true;
            }

            if (display.keyHasEvent(settings.get("Left"), jGL::EventType::PRESS))
            {
                state.events[Event::LEFT] = true;
            }

            if (display.keyHasEvent(settings.get("Right"), jGL::EventType::PRESS))
            {
                state.events[Event::RIGHT] = true;
            }

            if (display.keyHasEvent(settings.get("Left rotate"), jGL::EventType::PRESS))
            {
                state.events[Event::ROT_LEFT] = true;
            }

            if (display.keyHasEvent(settings.get("Right rotate"), jGL::EventType::PRESS))
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
                    case -7:
                        settings.defaults();
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
                        settings.set(selectingKey, code.first);
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
            settings.tutorial,
            &run_lua_loop,
            frameId,
            begin
        );

        tp1 = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->setClear(backgroundColour(darkMode));
            jGLInstance->clear();

            tr0 = high_resolution_clock::now();

            rendering.setProjection(camera.getVP());
            rendering.draw(jGLInstance, &manager, nullptr); 

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
                    textColour(darkMode),
                    glm::bvec2(true,false)
                );
            }
            
            if (state.gameOver)
            {
                jGLInstance->text
                (
                    "Game Over\nScore: "+std::to_string(int(state.score))+"\nClears: "+std::to_string(int(state.clears))+"\nSpace to replay",
                    glm::vec2(resX*0.5f,resY-64.0f),
                    0.5f,
                    textColour(darkMode),
                    glm::bvec2(true,false)
                );
            }
            else
            {
                jGLInstance->text
                (
                    "Score: "+std::to_string(int(state.score))+"\nClears: "+std::to_string(int(state.clears)),
                    glm::vec2(resX*0.5f,resY-32.0f),
                    0.5f,
                    textColour(darkMode),
                    glm::bvec2(false,false)
                );
            }

            if (!settings.tutorial.isDone() && !displayingMenu)
            {
                jGLInstance->text
                (
                    settings.tutorial.getTip
                    (
                        "Press "+keyCodes.at(settings.get("Up")), 
                        "Press "+keyCodes.at(settings.get("Left")),  
                        "Press "+keyCodes.at(settings.get("Left rotate"))
                    ),
                    glm::vec2(resX*0.5f,resY*0.5f),
                    0.5f,
                    textColour(darkMode),
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
                    "\nMonitor: (" << mode->width << ", " << mode->height << ")" <<
                    "\nWork area: (" << wwidth << ", " << wheight << ")" <<
                    "\nSlept: " << waited << 
                    "\nThis is debug output, press F2 to dismiss";

                jGLInstance->text
                (
                    debugText.str(),
                    glm::vec2(64.0f,resY-64.0f),
                    0.5f,
                    textColour(darkMode)
                );

            }

            if (displayingMenu)
            {
                jGLInstance->text
                (
                    info,
                    glm::vec2(resX*0.5,64.0),
                    0.5f,
                    textColour(darkMode),
                    glm::bvec2(true,false)
                );
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
                        textColour(darkMode),
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
                        textColour(darkMode),
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
                        textColour(darkMode),
                        glm::bvec2(false,false)
                    );
                }

                for (std::string key : controls)
                {
                    int binding = settings.get(key);
                    jGLInstance->text
                    (
                        key,
                        glm::vec2(x,y),
                        0.5f,
                        textColour(darkMode),
                        glm::bvec2(false,false)
                    );

                    jGLInstance->text
                    (
                        keyCodes.at(binding),
                        glm::vec2(x+keySelectXGap,y),
                        0.5f,
                        textColour(darkMode),
                        glm::bvec2(false,false)
                    );

                    y -= keySelectHeight*1.5;
                }
                jGLInstance->text
                (
                    "Defaults",
                    glm::vec2(x,y),
                    0.5f,
                    textColour(darkMode),
                    glm::bvec2(false,false)
                );            
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
                        textColour(darkMode),
                        glm::bvec2(true,false)
                    );
                }
            }

            if (loadedIcons)
            {
                sprites->getSprite("darkIcon").update(jGL::Transform(menuX, darkY, 0.0f, menuScale));
            }
            else
            {
                jGLInstance->text
                (
                    "dark/light",
                    glm::vec2(menuX*resX,darkY*resY),
                    0.5f,
                    textColour(darkMode),
                    glm::bvec2(true,false)
                );
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
        
        if (display.keyHasEvent(GLFW_KEY_ESCAPE, jGL::EventType::PRESS))
        {
            world.reset();
        }

        display.loop();

        deltas[frameId] = elapsed_micros.count() * 1e-6;
        frameId = (frameId+1) % 60;
        
        begin = false;
        
    }

    jGLInstance->finish();

    // force some static strings to appear in the binary, as a message to anyone using strings or hexdump
    message();

    #ifdef WINDOWS
        timeEndPeriod(1);
    #endif

    return 0;
}
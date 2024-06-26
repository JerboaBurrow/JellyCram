#include <jni.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <math.h>
#include <algorithm>
#include <memory>

#include <jGL/orthoCam.h>
#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

#include <jLog/jLog.h>

#include <Object/entityComponentSystem.h>

#include <System/Physics/sPhysics.h>
#include <System/Rendering/sRender.h>
#include <System/Physics/sCollision.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Console/console.h>

#include <Util/util.h>

#include <jLog/jLog.h>

#include <gameState.h>
#include <loop_lua.h>
#include <setup_lua.h>
#include <android_setup_lua.h>

#include <headers/json.hpp>
using json = nlohmann::json;

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Id;

using jGL::OrthoCam;
using Hop::System::Rendering::sRender;

using Hop::System::Physics::CollisionDetector;
using Hop::System::Physics::CollisionResolver;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;
using Hop::System::Physics::CollisionPrimitive;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::PerlinSource;
using Hop::World::Boundary;
using Hop::World::AbstractWorld;
using Hop::World::TileWorld;
using Hop::World::MarchingWorld;

using jLog::INFO;
using jLog::WARN;

static std::shared_ptr<Boundary<double>> boundary = nullptr;
static std::shared_ptr<AbstractWorld> world = nullptr;
static std::shared_ptr<Hop::World::FixedSource> mapSource = nullptr;

static std::shared_ptr<EntityComponentSystem> manager = nullptr;

static std::shared_ptr<jLog::Log> hopLog = nullptr;

static std::shared_ptr<Hop::Console> jconsole = nullptr;
static std::shared_ptr<Hop::LuaExtraSpace> consoleSpace = nullptr;

static std::shared_ptr<OrthoCam> camera = nullptr;

static std::shared_ptr<jGL::jGLInstance> jgl = nullptr;

static std::shared_ptr<Tutorial> tutorial = nullptr;

static std::shared_ptr<JellyCramState> gameState = nullptr;

const unsigned subSample = 5;
const double cofr = 0.25;
const double surfaceFriction = 0.5;

static double xmax = 1.0;
static bool invertTapControls = false;
static bool invertSwipeControls = false;
static bool screenCentric = false;
static glm::vec2 centre(0.0);
static uint64_t gameTimeMillis = 0;
static bool darkMode = false;

std::string fixedLengthNumber(double x, unsigned length);
glm::vec4 textColour(bool darkMode);
glm::vec4 backgroundColour(bool darkMode);

std::string jstring2string(JNIEnv *env, jstring jStr) {
    
    if (!jStr) {return "";}

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

extern "C"
{

    void Java_app_jerboa_jellycram_Hop_setDarkMode(JNIEnv * env, jobject, jboolean v)
    {
        darkMode = v;
        if (jconsole) {
            double r, g, b;
            if (darkMode) {
                r = 1.0;
                g = 1.0;
                b = 1.0;
                jconsole->runString("previewColour = previewColourDarkMode");
            } else {
                r = 72.0/255.0;
                g = r;
                b = r;
                jconsole->runString("previewColour = previewColourLightMode");
            }

            if (manager->handleExists("preview")) {
                Id p = manager->idFromHandle("preview");
                cRenderable & c = manager->getComponent<cRenderable>(p);
                c.r = r;
                c.g = g;
                c.b = b;
            }
        }
    }

    jlong Java_app_jerboa_jellycram_Hop_getGameTimeMillis(JNIEnv * env, jobject)
    {
        return gameTimeMillis;
    }

    jlong Java_app_jerboa_jellycram_Hop_getScore(JNIEnv * env, jobject)
    {
        return gameState->score;
    }

    jlong Java_app_jerboa_jellycram_Hop_getClears(JNIEnv * env, jobject)
    {
        return gameState->clears;
    }

    jboolean Java_app_jerboa_jellycram_Hop_isGameOver(JNIEnv * env, jobject)
    {
        return gameState->gameOver;
    }

    jboolean Java_app_jerboa_jellycram_Hop_smasherHit(JNIEnv * env, jobject)
    {
        return gameState->smasherHit;
    }

    jboolean Java_app_jerboa_jellycram_Hop_smasherMissed(JNIEnv * env, jobject)
    {
        return gameState->smasherMissed;
    }

    jdouble Java_app_jerboa_jellycram_Hop_landingSpeed(JNIEnv * env, jobject)
    {
        return gameState->landingSpeed;
    }

    jboolean Java_app_jerboa_jellycram_Hop_landed(JNIEnv * env, jobject)
    {
        return gameState->landed;
    }

    jstring Java_app_jerboa_jellycram_Hop_getState(JNIEnv * env, jobject)
    {
        std::string dump = "null";
        if (gameState != nullptr)
        {
            json jstate = *gameState.get();
            dump = jstate.dump();
        }
        return env->NewStringUTF(dump.c_str());
    }

    void Java_app_jerboa_jellycram_Hop_initialise(
            JNIEnv *env,
            jobject /* this */,
            jint resX,
            jint resY,
            jboolean skipTutorial
        )
    {

        // android may store state between certain life-cycle events
        //  must be absolutely sure to delete the old memory otherwise
        //  we sometimes end up with a blank screen (but the game still runs!)
        boundary = nullptr;
        world = nullptr;
        mapSource = nullptr;
        manager = nullptr;
        hopLog = nullptr;
        jconsole = nullptr;
        consoleSpace = nullptr;
        camera = nullptr;
        jgl = nullptr;
        tutorial = nullptr;
        gameState = nullptr;
        // same for the game timer
        gameTimeMillis = 0;

        hopLog = std::make_shared<jLog::Log>();

        jconsole = std::make_shared<Hop::Console>(*hopLog.get());

        tutorial = std::make_shared<Tutorial>(skipTutorial);

        tutorial->collide = "Collide with the edges or other pieces\n and you lose control";
        tutorial->jiggle = "Fill a row to clear\n if the Jiggleometer is low";
        tutorial->smash = "Use the smasher to break\n the pieces!";
        tutorial->weaker = "The controls get weaker\n the longer you play!";

        gameState = std::make_shared<JellyCramState>();

        float ratio = float(resX) / float(resY);
        xmax = ratio;

        float Mx = ratio*16;

        centre.x = 0.5*ratio;
        centre.y = 0.5;

        gameState->lengthScale = xmax*3.0/(3*6);
        gameState->fullWidthBinSize = 6;

        gameState->resolution = glm::vec2(resX, resY);
        gameState->currentTorque *= 3.0;
        gameState->settleFrames = 90;
        gameState->y0 = 0.25;

        camera = std::make_shared<jGL::OrthoCam>(resX, resY, glm::vec2(0.0,0.0));

        boundary = std::make_shared<Hop::World::FiniteBoundary<double>>(0,16.0*0.25, Mx,10000,true,false,true,true);
        mapSource = std::make_unique<Hop::World::FixedSource>();

        jgl = std::make_shared<jGL::GL::OpenGLInstance>(glm::ivec2(resX, resY), 0);

        jgl->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
        jgl->setMSAA(0);

        world = std::make_shared<TileWorld>
        (
                2,
                camera.get(),
                16,
                0,
                mapSource.get(),
                boundary.get()
        );

        manager = std::make_shared<EntityComponentSystem>();

        sRender & rendering = manager->getSystem<sRender>();
        rendering.setDrawMeshes(true);

        // setup physics system
        sPhysics & physics = manager->getSystem<sPhysics>();
        physics.setTimeStep(1.0/(4.0*900.0));
        physics.setGravity(9.81, 0.0, -1.0);
        physics.setSubSamples(subSample);

        sCollision & collisions = manager->getSystem<sCollision>();

        auto det = std::make_unique<Hop::System::Physics::CellList>
        (
                27,
                Hop::Util::tupled(0.0,1.0),
                Hop::Util::tupled(0.0,1.0)
        );

        auto res = std::make_unique<Hop::System::Physics::SpringDashpot>
        (
                deltaPhysics*10.0,
                cofr,
                surfaceFriction
        );

        collisions.setDetector(std::move(det));
        collisions.setResolver(std::move(res));

        collisions.centreOn(world.get()->getMapCenter());

        consoleSpace = std::make_shared<Hop::LuaExtraSpace>();
        consoleSpace->ecs = manager.get();
        consoleSpace->world = world.get();
        consoleSpace->physics = &physics;
        consoleSpace->resolver = &collisions;

        jconsole->luaStore(consoleSpace.get());

        jconsole->runString("s = "+std::to_string(gameState->lengthScale/3.0));
        jconsole->runString("y0 = "+std::to_string(gameState->y0));
        jconsole->runString(setup_lua);
        jconsole->runString("xmax = "+std::to_string(ratio));
        jconsole->runString(android_setup_lua);
        jconsole->runString("previewIndex = math.random(#meshes-1)");
        jconsole->runString("lastPreviewIndex = -1");
        jconsole->runString("nextPiece = true");
        if (darkMode) {
            jconsole->runString("previewColour = previewColourDarkMode");
        } else{
            jconsole->runString("previewColour = previewColourLightMode");
        }
        hopLog->androidLog();
    }

    void run_lua_loop(Hop::Console & console, std::string script)
    {
        if (script == "loop.lua")
        {
            console.runString(loop_lua);
        }
    }

    void Java_app_jerboa_jellycram_Hop_screenCentric(JNIEnv *env, jobject, jboolean v)
    {
        screenCentric = v;
    }

    void Java_app_jerboa_jellycram_Hop_invertTapControls(JNIEnv *env, jobject, jboolean v)
    {
        invertTapControls = v;
    }

    void Java_app_jerboa_jellycram_Hop_invertSwipeControls(JNIEnv *env, jobject, jboolean v)
    {
        invertSwipeControls = v;
    }

    void Java_app_jerboa_jellycram_Hop_restart(JNIEnv *env, jobject)
    {
        gameTimeMillis = 0;
        gameState->restart(*manager, *jconsole);
    }

    void Java_app_jerboa_jellycram_Hop_tap(JNIEnv *env,
             jobject,
             float sx,
             float sy)
    {
        if (gameState->gameOver)
        {
            gameState->restart(*manager,*jconsole);
            gameTimeMillis = 0.0;
            return;
        }

        if (manager->hasComponent<cTransform>(gameState->current)) {

            glm::vec4 w = camera->screenToWorld(sx,sy);
            float x = w.x;
            float y = 1.0-w.y;

            const auto &c = manager->getComponent<cTransform>(gameState->current);

            glm::vec2 focus = glm::vec2(c.x, c.y);

            if (screenCentric)
            {
                focus = centre;
            }

            if (x < focus.x-gameState->lengthScale)
            {
                if (invertTapControls)
                {
                    gameState->events[Event::LEFT] = true;
                }
                else
                {
                    gameState->events[Event::RIGHT] = true;
                }
            }
            else if (x > focus.x+gameState->lengthScale)
            {
                if (invertTapControls)
                {
                    gameState->events[Event::RIGHT] = true;
                }
                else
                {
                    gameState->events[Event::LEFT] = true;
                }
            }

            if (y < focus.y-gameState->lengthScale)
            {
                if (invertTapControls)
                {
                    gameState->events[Event::DOWN] = true;
                }
                else
                {
                    gameState->events[Event::UP] = true;
                }
            }
            else  if (y > focus.y+gameState->lengthScale)
            {
                if (invertTapControls)
                {
                    gameState->events[Event::UP] = true;
                }
                else
                {
                    gameState->events[Event::DOWN] = true;
                }
            }
        }
    }

    void Java_app_jerboa_jellycram_Hop_swipe(JNIEnv *env,
                 jobject,
                 float vx,
                 float vy)
    {
        if (invertSwipeControls)
        {
            if (vx < 0)
            {
                gameState->events[Event::ROT_RIGHT] = true;
            }
            else if (vx > 0)
            {
                gameState->events[Event::ROT_LEFT] = true;
            }
        }
        else
        {
            if (vx < 0)
            {
                gameState->events[Event::ROT_LEFT] = true;
            }
            else if (vx > 0)
            {
                gameState->events[Event::ROT_RIGHT] = true;
            }
        }
    }

    void Java_app_jerboa_jellycram_Hop_pause(JNIEnv *env, jobject, jboolean v)
    {
        if (v)
        {
            gameState->pause(*manager, *jconsole);
        }
        else
        {
            gameState->unpause(*manager, *jconsole);
        }
    }

    jboolean Java_app_jerboa_jellycram_Hop_tutorialDone(JNIEnv *env, jobject)
    {
        return tutorial->isDone();
    }

    void Java_app_jerboa_jellycram_Hop_loop
    (
            JNIEnv *env,
            jobject,
            jint frameId,
            jboolean first
    )
    {
        auto tic = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
        sRender & rendering = manager->getSystem<sRender>();
        sPhysics & physics = manager->getSystem<sPhysics>();
        sCollision & collisions = manager->getSystem<sCollision>();
        rendering.setDrawMeshes(true);

        gameState->iteration
        (
                *manager.get(),
                *jconsole.get(),
                collisions,
                physics,
                world,
                *tutorial,
                &run_lua_loop,
                frameId,
                first
        );
        jconsole->runString("nextX = nil");

        jgl->beginFrame();

            jgl->setClear(backgroundColour(darkMode));
            jgl->clear();

            rendering.setProjection(camera->getVP());
            rendering.draw(jgl, manager.get(), nullptr);

            if (!gameState->gameOver && gameState->incoming && !gameState->paused)
            {
                double t = gameState->countDownSeconds-gameState->elapsed_countdown;
                t = std::floor(t * 100.0)/100.0;
                jgl->text
                        (
                                fixedLengthNumber(t, 4),
                                glm::vec2(gameState->resolution.x*0.5f,gameState->resolution.y*0.125f),
                                0.3f*t,
                                textColour(darkMode),
                                glm::bvec2(true,false)
                        );
            }

            if (gameState->gameOver && !gameState->paused)
            {
                jgl->text
                (
                        "Game Over\nScore: "+std::to_string(int(gameState->score))+"\nClears: "+std::to_string(int(gameState->clears))+"\nTap to replay",
                        glm::vec2(gameState->resolution.x*0.5f,gameState->resolution.y*0.175f),
                        1.0f,
                        textColour(darkMode),
                        glm::bvec2(true,false)
                );
            }
            else if (!gameState->paused)
            {
                jgl->text
                (
                        "Score: "+std::to_string(int(gameState->score))+"\nClears: "+std::to_string(int(gameState->clears)),
                        glm::vec2(gameState->resolution.x*0.5f,gameState->resolution.y*0.175f),
                        1.0f,
                        textColour(darkMode),
                        glm::bvec2(true,false)
                );
            }

            if (!tutorial->isDone() && !gameState->paused)
            {
                std::string x, y, theta;

                if (invertSwipeControls)
                {
                    theta = "Swipe right\n";
                }
                else
                {
                    theta = "Swipe left\n";
                }

                if (screenCentric)
                {
                    if (invertTapControls)
                    {
                        y = "Tap the top-center of the screen\n";
                        x = "Tap the centre-left of the screen\n";
                    }
                    else
                    {
                        y = "Tap the bottom-centre of the screen\n";
                        x = "Tap the centre-right of the screen\n";
                    }
                }
                else
                {
                    if (invertTapControls)
                    {
                        y = "Tap above the piece\n";
                        x = "Tap the left of the piece\n";
                    }
                    else
                    {
                        y = "Tap below the piece\n";
                        x = "Tap the right of the piece\n";
                    }
                }
                jgl->text
                (
                    tutorial->getTip
                    (
                        y,
                        x,
                        theta
                    ),
                    glm::vec2(gameState->resolution.x*0.5f,gameState->resolution.y*0.75f),
                    1.0f,
                    textColour(darkMode),
                    glm::bvec2(true,true)
                );
            }

        jgl->endFrame();
        auto toc = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
        if (!gameState->gameOver && !gameState->paused)
        {
            gameTimeMillis += toc-tic;
        }
    }

    void Java_app_jerboa_jellycram_Hop_printLog
    (
            JNIEnv *env,
            jobject /* this */
    )
    {
        if (hopLog != nullptr)
        {
            hopLog->androidLog();
        }
    }

    void Java_app_jerboa_jellycram_Hop_beginFrame(JNIEnv * env, jobject /* this */)
    {
        jgl->beginFrame();
    }

    void Java_app_jerboa_jellycram_Hop_endFrame(JNIEnv * env, jobject /* this */)
    {
        jgl->endFrame();
    }

    void Java_app_jerboa_jellycram_Hop_render(
        JNIEnv *env,
        jobject /* this */,
        jboolean refresh
    )
    {
        if (manager == nullptr || jgl == nullptr || world == nullptr){
            return;
        }

        sRender & rendering = manager->getSystem<sRender>();

        rendering.setProjection(camera->getVP());
        rendering.draw(jgl, manager.get(), world.get());

    }

    void Java_app_jerboa_jellycram_Hop_renderText
    (
            JNIEnv *env,
            jobject /* this */,
            jstring text,
            jfloat x,
            jfloat y,
            jfloat scale,
            jfloat r,
            jfloat g,
            jfloat b,
            jfloat a,
            jboolean centredx,
            jboolean centredy
    )
    {
        if (jgl == nullptr){
            return;
        }

        jgl->text
        (
                jstring2string(env,text).c_str(),
                glm::vec2(x, y),
                scale,
                glm::vec4(r,g,b,a),
                glm::bvec2(centredx, centredy)
        );
    }

    void Java_app_jerboa_jellycram_Hop_getWorldPosition
    (
            JNIEnv *env,
            jobject /* this */
    );

    void Java_app_jerboa_jellycram_Hop_screenToWorld
    (
            JNIEnv *env,
            jobject /* this */,
            jfloat x,
            jfloat y,
            jfloat rx,
            jfloat ry
    ) {
        if (camera == nullptr) {
            return;
        }

        glm::vec4 w = camera->screenToWorld(x,y);
        rx = w[0];
        ry = w[1];
    }
}
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

static std::shared_ptr<JellyCramState> gameState;

const unsigned subSample = 5;
const double cofr = 0.25;
const double surfaceFriction = 0.5;

static double xmax = 1.0;

std::string fixedLengthNumber(double x, unsigned length);

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
            jint resY
        )
    {

        hopLog = std::make_shared<jLog::Log>();
        jconsole = std::make_shared<Hop::Console>(*hopLog.get());

        gameState = std::make_shared<JellyCramState>();

        float ratio = float(resX) / float(resY);
        xmax = ratio;

        float Mx = ratio*16;

        gameState->lengthScale = xmax*3.0/(3*6);
        gameState->fullWidthBinSize = 6;

        gameState->resolution = glm::vec2(resX, resY);
        gameState->currentTorque *= 3.0;
        gameState->settleFrames = 60;
        gameState->y0 = 0.25;

        camera = std::make_shared<jGL::OrthoCam>(resX, resY, glm::vec2(0.0,0.0));

        boundary = std::make_shared<Hop::World::FiniteBoundary<double>>(0,16.0*0.25, Mx,10000,true,false,true,true);
        mapSource = std::make_unique<Hop::World::FixedSource>();

        jgl = std::make_shared<jGL::GL::OpenGLInstance>(glm::ivec2(resX, resY), 0);

        jgl->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
        jgl->setMSAA(0);
        jgl->setClear(glm::vec4(1.0f,1.0f,1.0f,1.0f));

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

        jconsole->runString("nextX = 0.5;");
        jconsole->runString("xmax = "+std::to_string(ratio));
        jconsole->runString("s = "+std::to_string(gameState->lengthScale/3.0));
        jconsole->runString(setup_lua);
        hopLog->androidLog();
    }

    void run_lua_loop(Hop::Console & console, std::string script)
    {
        if (script == "loop.lua")
        {
            console.runString(loop_lua);
        }
    }

    void Java_app_jerboa_jellycram_Hop_tap(JNIEnv *env,
             jobject,
             float sx,
             float sy)
    {
        if (manager->hasComponent<cTransform>(gameState->current)) {

            glm::vec4 w = camera->screenToWorld(sx,sy);
            float x = w.x;
            float y = 1.0-w.y;

            const auto &c = manager->getComponent<cTransform>(gameState->current);

            if (x < c.x-gameState->lengthScale*0.5)
            {
                gameState->events[Event::RIGHT] = true;
            }
            else if (x > c.x+gameState->lengthScale*0.5)
            {
                gameState->events[Event::LEFT] = true;
            }

            if (y < c.y-gameState->lengthScale*0.5)
            {
                gameState->events[Event::UP] = true;
            }
            else  if (y > c.y+gameState->lengthScale*0.5)
            {
                gameState->events[Event::DOWN] = true;
            }
        }
    }

    void Java_app_jerboa_jellycram_Hop_swipe(JNIEnv *env,
             jobject,
             float vx,
             float vy)
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

    void Java_app_jerboa_jellycram_Hop_loop
    (
            JNIEnv *env,
            jobject,
            jint frameId,
            jboolean first
    )
    {
        sRender & rendering = manager->getSystem<sRender>();
        sPhysics & physics = manager->getSystem<sPhysics>();
        sCollision & collisions = manager->getSystem<sCollision>();
        rendering.setDrawMeshes(true);

        if (frameId == 0) {
            for (auto o : gameState->objects)
            {
                INFO(std::to_string(o)) >> *hopLog.get();
            }
        }
        gameState->iteration
        (
                *manager.get(),
                *jconsole.get(),
                collisions,
                physics,
                world,
                &run_lua_loop,
                frameId,
                first
        );

        jgl->beginFrame();

            jgl->clear();

            rendering.setProjection(camera->getVP());
            rendering.draw(jgl, manager.get(), world.get());

            if (!gameState->gameOver && gameState->incoming && !gameState->paused)
            {
                double t = gameState->countDownSeconds-gameState->elapsed_countdown;

                t = std::floor(t * 100.0)/100.0;
               jgl->text
                        (
                                fixedLengthNumber(t, 4),
                                glm::vec2(gameState->resolution.x*0.5f,gameState->resolution.y*0.2f),
                                0.3f*t,
                                glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                                glm::bvec2(true,false)
                        );
            }

            if (gameState->gameOver)
            {
                jgl->text
                (
                        "Game Over\nScore: "+std::to_string(int(gameState->score))+"\nTap to replay",
                        glm::vec2(gameState->resolution.x*0.5f,gameState->resolution.y*0.2f),
                        1.0f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(true,false)
                );
            }
            else
            {
                jgl->text
                (
                        "Score: "+std::to_string(int(gameState->score)),
                        glm::vec2(gameState->resolution.x*0.5f,32.0f),
                        1.0f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(true,false)
                );

                jgl->text
                (
                        std::to_string(int(gameState->settledFor)),
                        glm::vec2(gameState->resolution.x*0.5f,96.0f),
                        1.0f,
                        glm::vec4(0.0f,0.0f,0.0f, 1.0f),
                        glm::bvec2(true,false)
                );
            }

        jgl->endFrame();
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
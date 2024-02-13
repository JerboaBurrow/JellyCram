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

static std::shared_ptr<Boundary> boundary = nullptr;
static std::shared_ptr<PerlinSource> perlin = nullptr;
static std::shared_ptr<AbstractWorld> world = nullptr;

static std::shared_ptr<EntityComponentSystem> manager = nullptr;

static std::shared_ptr<jLog::Log> hopLog = nullptr;

static std::shared_ptr<Hop::Console> console = nullptr;

static std::unique_ptr<CollisionDetector> detector = nullptr;
static std::unique_ptr<CollisionResolver> resolver = nullptr;

static std::shared_ptr<sRender> renderer = nullptr;
static std::shared_ptr<OrthoCam> camera = nullptr;

static std::shared_ptr<jGL::jGLInstance> jgl = nullptr;

static std::shared_ptr<JellyCramState> gameState;

const unsigned subSample = 5;
const double cofr = 0.25;
const double surfaceFriction = 0.5;

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
            dump = jstate;
        }
        return env->NewStringUTF(dump.c_str());
    }

    void Java_app_jerboa_jellycram_Hop_initialise(
            JNIEnv *env,
            jobject /* this */,
            jstring state,
            jint resX,
            jint resY
        )
    {

        gameState = std::make_shared<JellyCramState>();
        json jstate = jstring2string(env, state);
        gameState->from_json(jstate);

        float posX = 0.0;
        float posY = 0.0;

        camera = std::make_shared<jGL::OrthoCam>(resX, resY);

        boundary = std::make_shared<Hop::World::FiniteBoundary>(0,0,16,16,true,false,true,true);
        Hop::World::FixedSource mapSource;

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
                &mapSource,
                boundary.get()
        );

        hopLog = std::make_shared<jLog::Log>();

        manager = std::make_shared<EntityComponentSystem>();

        sRender & rendering = manager->getSystem<sRender>();
        rendering.setDrawMeshes(true);

        // setup physics system
        sPhysics & physics = manager->getSystem<sPhysics>();
        physics.setTimeStep(1.0/900.0);
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

        hopLog = std::make_shared<jLog::Log>();

        console = std::make_shared<Hop::Console>(*hopLog.get());

        Hop::LuaExtraSpace luaStore;
        luaStore.ecs = manager.get();
        luaStore.world = world.get();
        luaStore.physics = &physics;
        luaStore.resolver = &collisions;

        console->luaStore(&luaStore);
    }

    void Java_app_jerboa_jellycram_Hop_loop(JNIEnv *env, jobject, jboolean first)
    {

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
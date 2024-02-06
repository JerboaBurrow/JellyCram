#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <time.h>
#include <random>
#include <math.h>
#include <vector>

#include <chrono>
using namespace std::chrono;

#include <logo.h>

#include <Object/entityComponentSystem.h>

#include <System/Physics/sPhysics.h>
#include <System/Rendering/sRender.h>
#include <System/Physics/sCollision.h>
#include <System/Sound/sSound.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Console/console.h>

#include <Debug/collisionMeshDebug.h>

#include <jLog/jLog.h>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

bool debug = true;
bool paused = false;

const double deltaPhysics = 1.0/900.0;
const double gravity = 9.81;
const double impulse = gravity*0.9*150.0;
const double torque = 3.14*100000;

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Id;

using Hop::System::Rendering::sRender;

using Hop::System::Physics::CollisionDetector;
using Hop::System::Physics::CollisionResolver;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;
using Hop::System::Sound::sSound;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::Boundary;
using Hop::World::AbstractWorld;
using Hop::World::TileWorld;
using jLog::INFO;
using jLog::WARN;

std::string fixedLengthNumber(double x, unsigned length)
{
    std::string d = std::to_string(x);
    std::string dtrunc(length,' ');
    for (unsigned c = 0; c < dtrunc.length(); c++/*ayy lmao*/)
    {

        if (c >= d.length())
        {
            dtrunc[c] = '0';
        }
        else
        {
            dtrunc[c] = d[c];
        }
    }
    return dtrunc;
}

std::shared_ptr<jGL::jGLInstance> jGLInstance;

bool objectOverTop(const cCollideable & o, double topy)
{
    const auto & bb = o.mesh.getBoundingBox();
    return bb.ul.y > topy || bb.ur.y > topy || bb.ll.y > topy || bb.lr.y > topy;
}

void fadeAll(std::vector<Id> & objects, EntityComponentSystem & manager, double a)
{
    for (auto & o : objects)
    {
        auto & r = manager.getComponent<cRenderable>(o);
        r.a = a;
    }
}

void checkDelete(std::vector<Id> & objects, EntityComponentSystem & manager, double r, uint8_t binSize)
{

    std::multimap<uint64_t, std::pair<Id, uint64_t>> bins;  

    for (const auto & o : objects)
    {
        auto & c = manager.getComponent<cCollideable>(o);

        for (uint64_t t : c.mesh.getTags())
        {
            uint64_t bin = uint64_t(c.mesh.getBoundingBox(t).centre.y/r);
            bins.insert(std::pair(bin, std::pair(o, t)));
        }
    }

    uint64_t maxKey = 1.0/r;

    for (uint64_t k = 0; k < maxKey; k++)
    {
        auto range = bins.equal_range(k);
        if (std::distance(range.first, range.second) >= binSize)
        {
            for (auto iter = range.first; iter != range.second; iter++)
            {
                Id id = iter->second.first;
                uint64_t tag = iter->second.second;
                
                cCollideable & c = manager.getComponent<cCollideable>(id);
                c.mesh.removeByTag(tag);

                if (c.mesh.size() == 0)
                {
                    manager.remove(id);
                    auto it = std::find(objects.begin(), objects.end(), id);
                    if (it != objects.end())
                    {
                        objects.erase(it);
                    }
                }
            }
        }
    }

    
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void message()
{
	__attribute__((unused)) static const char * message = "// ******************************** Hello to the data miners, modders, and explorers, you are welcome to poke around! The code is OSS though ;) \n Checkout the repo for modding tips https://github.com/JerboaBurrow/JellyCram ********************************";
}

#pragma GCC pop_options

#endif /* MAIN_H */

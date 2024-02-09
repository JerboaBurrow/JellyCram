#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>

#include <time.h>
#include <random>
#include <math.h>
#include <vector>

#include <chrono>
using namespace std::chrono;

#include <icon.h>
#include <icon2.h>

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

#include <gameState.h>

bool debug = true;
bool paused = false;

std::random_device rngDevice;
std::default_random_engine rng(rngDevice());
std::uniform_int_distribution<uint8_t> RNGU8(1, 4);
std::uniform_real_distribution<double> RNGR;

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

std::vector<std::pair<Id, uint64_t>> deleteQueue;
std::vector<Id> deleteQueueIds;

std::vector<Id> objects;

double fx, fy, omega;

Id current;

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

double pickX(std::vector<Id> & objects, uint8_t bins, double r, EntityComponentSystem & manager)
{
    std::vector<uint16_t> counts = std::vector<uint16_t>(uint16_t(1.0/r), 1);

    for (auto o : objects)
    {
        const auto & c = manager.getComponent<cCollideable>(o);
        counts[size_t(c.mesh.getBoundingBox().centre.x/r)]+=3;
    }

    unsigned sum = 0;
    for (auto i : counts)
    {
        sum += i;
    }

    std::vector<double> weights(counts.size(),0.0);
    
    std::transform(counts.begin(), counts.end(), weights.begin(), [sum](uint16_t c) {return double(c/sum);});

    std::discrete_distribution<int> distribution(weights.begin(), weights.end()); 

    int bin = distribution(rng);

    return bin*r;
}

std::vector<std::pair<Id, uint64_t>> checkDelete(std::vector<Id> & objects, EntityComponentSystem & manager, double r, uint8_t binSize)
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

    std::vector<std::pair<Id, uint64_t>> toDelete;

    for (uint64_t k = 0; k < maxKey; k++)
    {
        auto range = bins.equal_range(k);
        
        if (std::distance(range.first, range.second) >= binSize)
        {
            for (auto iter = range.first; iter != range.second; iter++)
            {
                toDelete.push_back(iter->second);
            }
        }
    }

    return toDelete;

}

void handleDelete(std::vector<std::pair<Id, uint64_t>> & toDelete, std::vector<Id> & objects, EntityComponentSystem & manager)
{

    for (auto p : toDelete)
    {
        cCollideable & c = manager.getComponent<cCollideable>(p.first);
        c.mesh.removeByTag(p.second);

        if (c.mesh.size() == 0)
        {
            manager.remove(p.first);
            auto it = std::find(objects.begin(), objects.end(), p.first);
            if (it != objects.end())
            {
                objects.erase(it);
            }
        }
        else
        {
            /*
                Check for contiguity

                    Assume bounding boxes of equal side lengths w
                    
                    x x
                    x x

                    The centre of a box must be within w units to be
                    contiguous (for rigid bodies)

                    Contiguity parameter is,

                    c := dij / w

                    For rigid bodies c <= 1 is contiguous

                    For soft we have c ~ 1

            */ 
            if (c.mesh.getTags().size() > 1)
            {
                for (auto ti : c.mesh.getTags())
                {
                    auto bbi = c.mesh.getBoundingBox(ti);  
                    double w = Hop::Maths::norm(bbi.ll-bbi.lr);
                    if (c.mesh.getTags().size() > 1)
                    {
                        bool contiguous = false;
                        for (auto tj : c.mesh.getTags())
                        {
                            if (ti != tj)
                            {
                                auto bbj = c.mesh.getBoundingBox(tj);
                                double d = Hop::Maths::norm(bbi.centre-bbj.centre);
                                // less than 5% discontiguous
                                if (d / w < 1.05)
                                {
                                    contiguous = true;
                                    break;
                                }
                            }
                        }
                        if (!contiguous)
                        {
                            // remove this piece, ti, into a new object
                            double x = bbi.centre.x;
                            double y = bbi.centre.y;
                            auto trans = manager.getComponent<cTransform>(p.first);
                            auto ren = manager.getComponent<cRenderable>(p.first);
                            auto phys = manager.getComponent<cPhysics>(p.first);

                            phys.lastX = x;
                            phys.lastY = y;

                            Hop::System::Physics::CollisionMesh nm = c.mesh.getSubMesh(ti);
                            c.mesh.removeByTag(ti);

                            auto bb = c.mesh.getBoundingBox().centre;
                            auto & transO = manager.getComponent<cTransform>(p.first);
                            auto & physO = manager.getComponent<cPhysics>(p.first);
                            transO.x = bb.x;
                            transO.y = bb.y;
                            physO.lastX = bb.x;
                            physO.lastY = bb.y;
                            c.mesh.reinitialise();

                            ren.r = RNGR(rng);
                            ren.g = RNGR(rng);
                            ren.b = RNGR(rng);

                            Id nid = manager.createObject();
                            manager.addComponent<cTransform>(nid, cTransform(x,y,trans.theta,trans.scale));
                            manager.addComponent<cRenderable>(nid, ren);
                            manager.addComponent<cPhysics>(nid, phys);
                            manager.addComponent<cCollideable>(nid, cCollideable(nm));
                            objects.push_back(nid);

                            manager.getComponent<cCollideable>(nid).mesh.reinitialise();
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    toDelete.clear();
    return;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void message()
{
	__attribute__((unused)) static const char * message = "// ******************************** Hello to the data miners, modders, and explorers, you are welcome to poke around! The code is OSS though ;) \n Checkout the repo for modding tips https://github.com/JerboaBurrow/JellyCram ********************************";
}

#pragma GCC pop_options

#endif /* MAIN_H */
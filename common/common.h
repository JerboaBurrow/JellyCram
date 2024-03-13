#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <time.h>
#include <math.h>
#include <vector>

#include <chrono>
using namespace std::chrono;

#include <icon.h>
#include <icon2.h>

#include <Debug/collisionMeshDebug.h>
#include <Object/entityComponentSystem.h>
#include <Object/id.h>
#include <World/tileWorld.h>

#include <jLog/jLog.h>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>

#include <rand.h>

using Hop::Object::Id;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;

using Hop::System::Rendering::sRender;

using Hop::System::Physics::CollisionDetector;
using Hop::System::Physics::CollisionResolver;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::Boundary;
using Hop::World::TileWorld;
using jLog::INFO;
using jLog::WARN;

std::string fixedLengthNumber(double x, unsigned length);

void smash(Id with, std::vector<Id> & objects, EntityComponentSystem & ecs);

bool objectOverTop(const cCollideable & o, double topy);

void fadeAll(std::vector<Id> & objects, EntityComponentSystem & manager, double a);

double pickX(std::vector<Id> & objects, uint8_t bins, double r, double xmax, EntityComponentSystem & manager);

std::vector<std::pair<Id, uint64_t>> checkDelete(std::vector<Id> & objects, EntityComponentSystem & manager, double r, uint8_t binSize, double y0 = 0.0);

void handleDelete
(
    std::vector<std::pair<Id, uint64_t>> & toDelete, 
    std::vector<Id> & objects, 
    EntityComponentSystem & manager,
    double outOfPlayFade = 0.5
);

double energy(std::vector<Id> & objects, EntityComponentSystem & manager);

#pragma GCC push_options
#pragma GCC optimize ("O0")
void message();
#pragma GCC pop_options

#endif /* COMMON_H */

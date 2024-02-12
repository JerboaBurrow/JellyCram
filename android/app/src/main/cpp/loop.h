//
// Created by jerboa on 09/02/2024.
//

#ifndef JELLYCRAM_LOOP_H
#define JELLYCRAM_LOOP_H

#include "gameState.h"

#include <random>
#include <vector>
#include <chrono>
using namespace std::chrono;

#include <Object/entityComponentSystem.h>
#include <System/Physics/sPhysics.h>
#include <System/Rendering/sRender.h>
#include <System/Physics/sCollision.h>
#include <jLog/jLog.h>
#include <Console/console.h>
#include <jGL/jGL.h>

#include <scripts/loop_lua.h>
#include <scripts/meshes_lua.h>

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;
using Hop::System::Physics::CollisionPrimitive;
using Hop::World::AbstractWorld;
using Hop::Object::Id;

const double gravity = 9.81;
const double impulse = gravity*0.9*150.0;
const double torque = 3.14*100000;
const double minCountdown = 0.5;
const double minImpulse = impulse*0.5;
const double minTorque = torque*0.25;
const double pulseFreq = 0.33;

std::random_device rngDevice;
std::default_random_engine rng(rngDevice());
std::uniform_int_distribution<uint8_t> RNGU8(1, 4);
std::uniform_real_distribution<double> RNGR;

uint64_t score = 0;
uint64_t graceFrames = 60;

double fx = 0.0;
double fy = 0.0;
double omega = 0.0;

double countDownSeconds = 5.0;
double countDownDecrement = 0.1;
double elapsed_countdown = 0.0;
high_resolution_clock::time_point countDownBegin;

double impulseSoftening = 0.975;
double torqueSoftening = 0.975;
double currentTorque = torque;
double currentImpulse = impulse;

double deletePulseTimeSeconds = 1.5;
high_resolution_clock::time_point deletePulseBegin;

std::vector<std::pair<Id, uint64_t>> deleteQueue;
std::vector<Id> deleteQueueIds;

std::vector<Id> objects;
Id current;

void gameLoop
(
    std::shared_ptr<JellyCramState> state,
    std::shared_ptr<EntityComponentSystem> manager,
    std::shared_ptr<jGL::jGLInstance> instance,
    std::shared_ptr<AbstractWorld> world,
    std::shared_ptr<Hop::Console> console,
    std::shared_ptr<jLog::Log> log,
    bool first = false
)
{
    if (first)
    {
        state->allowMove = true;
        state->incoming = false;
        state->gameOver = false;
        state->score = 0;
        state->paused = false;
        console->runString(meshes_lua);
        console->runString("math.randomseed(os.time())");
        console->runString("previewIndex = math.random(#meshes)");
        console->runString("nextX = 0.5;");
    }

    fx = 0.0;
    fy = 0.0;
    omega = 0.0;

    if (!state->paused)
    {
        console->runString(loop_lua);
    }

    Id id = manager->idFromHandle("current");

    if (first)
    {
        current = id;
        objects.push_back(current);
    }

    if (!first && id != current)
    {
        objects.push_back(id);
        state->allowMove = true;
        current = id;
        score += 1; // all tetrominoes have the same number of 3x3 blocks
        console->runString("previewIndex = math.random(#meshes)");
        console->runString("nextX = "+std::to_string(pickX(objects, 9, 3.0/27.0, manager)));
        countDownSeconds = std::max(countDownSeconds-countDownDecrement, minCountdown);
        currentImpulse = std::max(impulseSoftening*currentImpulse, minImpulse);
        currentTorque = std::max(torqueSoftening*currentTorque, minTorque);
    }
}

#endif //JELLYCRAM_LOOP_H

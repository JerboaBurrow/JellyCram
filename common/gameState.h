#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <common.h>

#include <Object/entityComponentSystem.h>
#include <Object/id.h>
#include <System/Physics/sPhysics.h>
#include <System/Rendering/sRender.h>
#include <System/Physics/sCollision.h>
#include <World/world.h>
#include <Console/console.h>

#include <headers/json.hpp>
using json = nlohmann::json;

#include <string>
#include <limits>
#include <map>
#include <random>

const bool develop = true;

const double deltaPhysics = 1.0/900.0;
const double gravity = 9.81;
const double impulse = gravity*0.9*150.0;
const double torque = 3.14*100000;

const double minCountdown = 0.5;
const double minImpulse = impulse*0.5;
const double minTorque = torque*0.25;

const double pulseFreq = 0.33;
const double deletePulseTimeSeconds = 1.5;

const double countDownDecrement = 0.1;
const double impulseSoftening = 0.975;
const double torqueSoftening = 0.975;

enum class Event {UP, DOWN, LEFT, RIGHT, ROT_LEFT, ROT_RIGHT, PAUSE};

using Hop::Object::Id;
using Hop::Object::EntityComponentSystem;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;
using Hop::World::AbstractWorld;

struct JellyCramState
{

    JellyCramState()
    : gameOver(false), 
      incoming(false), 
      allowMove(true), 
      paused(false),
      debug(false),
      graceFrames(60),
      countDownSeconds(5.0),
      elapsed_countdown(0.0),
      countDownBegin(),
      deletePulseBegin(),
      currentImpulse(impulse),
      currentTorque(torque),
      score(0u),
      events()
    {}

    bool gameOver;
    bool incoming;
    bool allowMove;
    bool paused;
    bool debug;

    uint8_t graceFrames;

    double countDownSeconds;
    double elapsed_countdown;
    uint64_t countDownBegin;
    uint64_t deletePulseBegin;

    double currentImpulse;
    double currentTorque;

    uint32_t score;
    std::map<Event, bool> events;

    std::vector<std::pair<Id, uint64_t>> deleteQueue;
    std::vector<Id> deleteQueueIds;

    std::vector<Id> objects;

    Id current;

    void iteration
    (
        EntityComponentSystem & ecs,
        Hop::Console & console,
        sCollision & collisions,
        sPhysics & physics,
        std::shared_ptr<AbstractWorld> world,
        uint8_t frameId,
        bool begin = false
    );
};

void to_json(json & j, const JellyCramState & s);

void from_json(const json & j, JellyCramState & s);

JellyCramState fromJson(std::string j);

std::string toJson(JellyCramState & state);

#endif /* GAMESTATE_H */

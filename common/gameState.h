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

const bool develop = false;

const double deltaPhysics = 1.0/900.0;
const double gravity = 9.81;
const double impulse = gravity*0.9*150.0;
const double torque = 3.14*100000;

const double minCountdown = 0.5;
const double minImpulse = impulse*0.5;
const double minTorque = torque*0.25;

const double pulseFreq = 0.33;
const double deletePulseTimeSeconds = 3.0;

const double countDownDecrement = 0.1;
const double impulseSoftening = 0.975;
const double torqueSoftening = 0.975;

const double settleThreshold = 0.5;
const double settleDifficuty = 0.95;
const double minSettleThreshold = 0.01;

const double outOfPlayFade = 0.5;

enum class Event {UP, DOWN, LEFT, RIGHT, ROT_LEFT, ROT_RIGHT, PAUSE};

using Hop::Object::Id;
using Hop::Object::EntityComponentSystem;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;
using Hop::World::AbstractWorld;

typedef void (* run_lua) (Hop::Console & console, std::string script);

void run_lua_file(Hop::Console & console, std::string script);
void run_lua_packed(Hop::Console & console, std::string script);

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
      currentSettleThreshold(settleThreshold),
      lengthScale(3.0/27),
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
    double currentSettleThreshold;
    double lengthScale;

    uint32_t score;
    std::map<Event, bool> events;

    std::vector<std::pair<Id, uint64_t>> deleteQueue;
    std::vector<Id> deleteQueueIds;
    bool deleting = false;
    uint8_t fullWidthBinSize = 9;

    std::vector<Id> objects;

    Id current;

    std::vector<double> jiggleometerAlphas = std::vector<double>(10,1.0);

    void iteration
    (
        EntityComponentSystem & ecs,
        Hop::Console & console,
        sCollision & collisions,
        sPhysics & physics,
        std::shared_ptr<AbstractWorld> world,
        run_lua lua_loop = &run_lua_file,
        uint8_t frameId = 0,
        bool begin = false
    );

    void from_json(const json & j) {

      j.at("gameOver").get_to(gameOver);
      j.at("allowMove").get_to(allowMove);
      j.at("incoming").get_to(incoming);
      j.at("paused").get_to(paused);
      j.at("debug").get_to(debug);
      j.at("graceFrames").get_to(graceFrames);
      j.at("countDownSeconds").get_to(countDownSeconds);
      j.at("elapsed_countdown").get_to(elapsed_countdown);
      j.at("countDownBegin").get_to(countDownBegin);
      j.at("deletePulseBegin").get_to(deletePulseBegin);
      j.at("currentImpulse").get_to(currentImpulse);
      j.at("currentTorque").get_to(currentTorque);
      j.at("currentSettleThreshold").get_to(currentSettleThreshold);
      j.at("lengthScale").get_to(lengthScale);
      j.at("score").get_to(score);
      j.at("events").get_to(events);

      std::string sid;
      j.at("current").get_to(sid);
      current = Id(sid);

      std::vector<std::pair<std::string, uint64_t>> jdeleteQueue;
      std::vector<std::string> jdeleteQueueIds;
      std::vector<std::string> jobjects;

      j.at("deleteQueue").get_to(jdeleteQueue);
      j.at("deleteQueueIds").get_to(jdeleteQueueIds);
      j.at("deleting").get_to(deleting);
      j.at("objects").get_to(jobjects);

      deleteQueue.resize(jdeleteQueue.size());
      deleteQueueIds.resize(jdeleteQueueIds.size());
      objects.resize(jobjects.size());

      std::transform
      (
          jdeleteQueue.begin(),
          jdeleteQueue.end(), 
          deleteQueue.begin(),
          [](std::pair<std::string, uint64_t> p) { return std::pair(Id(p.first), p.second);}
      );

      std::transform
      (
          jdeleteQueueIds.begin(),
          jdeleteQueueIds.end(), 
          deleteQueueIds.begin(),
          [](std::string i) { return Id(i);}
      );

      std::transform
      (
          jobjects.begin(),
          jobjects.end(), 
          objects.begin(),
          [](std::string i) { return Id(i);}
      );
  }
};

void to_json(json & j, const JellyCramState & s);

void from_json(const json & j, JellyCramState & s);

JellyCramState fromJson(std::string j);

std::string toJson(JellyCramState & state);

#endif /* GAMESTATE_H */

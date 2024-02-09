#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <json.hpp>
using json = nlohmann::json;

#include <string>
#include <limits>

enum class Event {UP, DOWN, LEFT, RIGHT, ROT_LEFT, ROT_RIGHT, PAUSE};

struct JellyCramState
{

    JellyCramState()
    : gameOver(false), 
      incoming(false), 
      allowMove(true), 
      paused(false),
      graceFrames(60),
      countDownSeconds(5.0),
      elapsed_countdown(0.0),
      currentImpulse(impulse),
      currentTorque(torque),
      score(0u),
      events()
    {}

    bool gameOver;
    bool incoming;
    bool allowMove;
    bool paused;

    uint8_t graceFrames;

    double countDownSeconds;
    double elapsed_countdown;
    double currentImpulse;
    double currentTorque;

    uint32_t score;
    std::vector<Event> events;
};

void to_json(json & j, const JellyCramState & s) {
    j = json
    {
        {"gameOver", s.gameOver}, 
        {"allowMove", s.allowMove}, 
        {"incoming", s.incoming},
        {"paused", s.paused},
        {"graceFrames", s.graceFrames},
        {"countDownSeconds", s.countDownSeconds},
        {"elapsed_countdown", s.elapsed_countdown},
        {"currentImpulse", s.currentImpulse},
        {"currentTorque", s.currentTorque},
        {"score", s.score},
        {"events", s.events}
    };
}

void from_json(const json & j, JellyCramState & s) {
    j.at("gameOver").get_to(s.gameOver);
    j.at("allowMove").get_to(s.allowMove);
    j.at("incoming").get_to(s.incoming);
    j.at("paused").get_to(s.paused);
    j.at("graceFrames").get_to(s.graceFrames);
    j.at("countDownSeconds").get_to(s.countDownSeconds);
    j.at("elapsed_countdown").get_to(s.elapsed_countdown);
    j.at("currentImpulse").get_to(s.currentImpulse);
    j.at("currentTorque").get_to(s.currentTorque);
    j.at("score").get_to(s.score);
    j.at("events").get_to(s.events);
}

JellyCramState fromJson(std::string j)
{
    json parsed = json::parse(j);

    JellyCramState state = parsed.template get<JellyCramState>();
}

std::string toJson(JellyCramState & state)
{
    return json(state).dump();
}

#endif /* GAMESTATE_H */

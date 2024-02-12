#include <gameState.h>

void JellyCramState::iteration
(
    EntityComponentSystem & ecs,
    Hop::Console & console,
    sCollision & collisions,
    sPhysics & physics,
    std::shared_ptr<AbstractWorld> world,
    uint8_t frameId,
    bool begin
)
{

    if (events[Event::PAUSE])
    {
        if (paused)
        {
            if (!develop){fadeAll(objects,ecs,0.75);}
            countDownBegin = high_resolution_clock::now().time_since_epoch().count();
        }
        else
        {
            if (!develop){fadeAll(objects,ecs,0.0);}
        }
        paused = !paused; 
        events[Event::PAUSE] = false;
    }

    if (!gameOver)
    {
        double fx = 0.0; 
        double fy = 0.0;
        double omega = 0.0;

        if (!paused)
        {
            console.runFile("loop.lua");
        }

        Id id = ecs.idFromHandle("current");

        if (begin)
        {
            current = id;
            objects.push_back(current);
        }

        if (!begin && id != current)
        {
            // a new piece
            objects.push_back(id);
            allowMove = true;
            current = id;
            score += 1; // all tetrominoes have the same number of 3x3 blocks
            console.runString("previewIndex = math.random(#meshes)");
            console.runString("nextX = "+std::to_string(pickX(objects, 9, 3.0/27.0, ecs)));
            countDownSeconds = std::max(countDownSeconds-countDownDecrement, minCountdown);
            currentImpulse = std::max(impulseSoftening*currentImpulse, minImpulse);
            currentTorque = std::max(torqueSoftening*currentTorque, minTorque);
        }

        if (collisions.objectHasCollided(current) != CollisionDetector::CollisionType::NONE)
        {
            if (collisions.objectHasCollided(current) == CollisionDetector::CollisionType::OBJECT)
            {
                if (current != Id(-1) && ecs.hasComponent<cCollideable>(current))
                {
                    const cCollideable & c = ecs.getComponent<cCollideable>(current);
                    if (objectOverTop(c, 1.0))
                    {
                        if (graceFrames <= 1)
                        {
                            gameOver = true;
                            fadeAll(objects,ecs,0.33);
                        }
                        else
                        {
                            graceFrames -= 1;
                        }
                    }
                }
            }

            if (allowMove) 
            { 
                cRenderable & r = ecs.getComponent<cRenderable>(current);
                // fade outslightly
                r.a = 0.75;
                incoming = true; 
                countDownBegin = high_resolution_clock::now().time_since_epoch().count();
            }
            allowMove = false;
        }

        if (events[Event::UP])
        {
            fy += currentImpulse;
            events[Event::UP] = false;
        }

        if (events[Event::DOWN])
        {
            fy -= currentImpulse;
            events[Event::DOWN] = false;
        }

        if (events[Event::LEFT])
        {
            fx -= currentImpulse;
            events[Event::LEFT] = false;
        }

        if (events[Event::RIGHT])
        {
            fx += currentImpulse;
            events[Event::RIGHT] = false;
        }

        if (events[Event::ROT_LEFT])
        {
            omega -= currentTorque;
            events[Event::ROT_LEFT] = false;
        }

        if (events[Event::ROT_RIGHT])
        {
            omega += currentTorque;
            events[Event::ROT_RIGHT] = false;
        }

        if (allowMove && (fx != 0.0 || fy != 0.0))
        {
            physics.applyForce
            (
                &ecs,
                id,
                fx,
                fy,
                true
            );
        }

        if (allowMove && omega != 0.0)
        {
            physics.applyTorque
            (
                &ecs,
                id,
                omega
            );
        }

        if (deleteQueue.size() > 0)
        {
            double t = high_resolution_clock::now().time_since_epoch().count()-deletePulseBegin;
            fadeAll(deleteQueueIds, ecs, std::abs(std::cos(t*pulseFreq*2.0*3.14159)));
            if (t >= deletePulseTimeSeconds)
            {
                fadeAll(deleteQueueIds, ecs, 1.0);
                handleDelete(deleteQueue, objects, ecs);
                deleteQueue.clear();
                deleteQueueIds.clear();
            }
        }

        if (frameId == 0 && deleteQueue.size() == 0)
        {
            deleteQueue = checkDelete(objects, ecs, 3.0/(3*9), 9);
            deleteQueueIds.clear();
            for (auto o : deleteQueue)
            {
                deleteQueueIds.push_back(o.first);
            }
            
            deletePulseBegin = high_resolution_clock::now().time_since_epoch().count();
        }

        if (incoming && !paused)
        {
            elapsed_countdown += high_resolution_clock::now().time_since_epoch().count()-countDownBegin;
            countDownBegin = high_resolution_clock::now().time_since_epoch().count();

            if (elapsed_countdown >= countDownSeconds)
            {
                incoming = false;
                console.runString("nextPiece = true");
                elapsed_countdown = 0.0;
            }
            else
            {
                double t = countDownSeconds-elapsed_countdown;
                t = std::floor(t * 100.0)/100.0;
            }
        }
    }

    if (!paused && deleteQueue.size() == 0)
    {
        physics.step(&ecs, &collisions, world.get());
    }
}

void to_json(json & j, const JellyCramState & s) {
    j = json
    {
        {"gameOver", s.gameOver}, 
        {"allowMove", s.allowMove}, 
        {"incoming", s.incoming},
        {"paused", s.paused},
        {"debug", s.debug},
        {"graceFrames", s.graceFrames},
        {"countDownSeconds", s.countDownSeconds},
        {"elapsed_countdown", s.elapsed_countdown},
        {"countDownBegin", s.countDownBegin},
        {"deletePulseBegin", s.deletePulseBegin},
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
    j.at("debug").get_to(s.debug);
    j.at("graceFrames").get_to(s.graceFrames);
    j.at("countDownSeconds").get_to(s.countDownSeconds);
    j.at("elapsed_countdown").get_to(s.elapsed_countdown);
    j.at("countDownBegin").get_to(s.countDownBegin);
    j.at("deletePulseBegin").get_to(s.deletePulseBegin);
    j.at("currentImpulse").get_to(s.currentImpulse);
    j.at("currentTorque").get_to(s.currentTorque);
    j.at("score").get_to(s.score);
    j.at("events").get_to(s.events);
}

JellyCramState fromJson(std::string j)
{
    json parsed = json::parse(j);

    JellyCramState state = parsed.template get<JellyCramState>();

    return state;
}

std::string toJson(JellyCramState & state)
{
    return json(state).dump();
}
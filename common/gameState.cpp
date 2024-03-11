#include <gameState.h>

void run_lua_file(Hop::Console & console, std::string script) { console.runFile(script); }
void run_lua_packed(Hop::Console & console, std::string script) { console.runScript(script); }
void run_lua_string(Hop::Console & console, std::string script) { console.runString(script); }

void JellyCramState::iteration
(
    EntityComponentSystem & ecs,
    Hop::Console & console,
    sCollision & collisions,
    sPhysics & physics,
    std::shared_ptr<AbstractWorld> world,
    run_lua lua_loop,
    uint8_t frameId,
    bool begin
)
{

    if (events[Event::PAUSE])
    {
        if (paused)
        {
            if (!develop){fadeAll(objects,ecs,outOfPlayFade);}
            countDownBegin = duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
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
            lua_loop(console, "loop.lua");
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
            console.runString("nextX = "+std::to_string(pickX(objects, fullWidthBinSize, lengthScale, 1.0, ecs)));
            countDownSeconds = std::max(countDownSeconds-countDownDecrement, minCountdown);
            currentImpulse = std::max(impulseSoftening*currentImpulse, minImpulse);
            currentTorque = std::max(torqueSoftening*currentTorque, minTorque);
            currentSettleThreshold = std::max(currentSettleThreshold*settleDifficuty, minSettleThreshold);
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
                r.a = outOfPlayFade;
                incoming = true; 
                countDownBegin = duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
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

        if (!deleting && allowMove && (fx != 0.0 || fy != 0.0))
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

        if (!deleting && allowMove && omega != 0.0)
        {
            physics.applyTorque
            (
                &ecs,
                id,
                omega
            );
        }

        double asd = lengthScale*lengthScale*energy(objects, ecs) / (1.0+objects.size());

        if (deleteQueue.size() > 0 && !deleting && asd < currentSettleThreshold*currentSettleThreshold*lengthScale*lengthScale)
        {
            deletePulseBegin = duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
            deleting = true;
        }

        if (deleteQueue.size() > 0 && deleting)
        {
            double t = double(duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count()-deletePulseBegin)*1e-9;
            fadeAll(deleteQueueIds, ecs, std::abs(std::cos(t*pulseFreq*2.0*3.14159)));
            if (t >= deletePulseTimeSeconds)
            {
                fadeAll(deleteQueueIds, ecs, 1.0);
                handleDelete(deleteQueue, objects, ecs, outOfPlayFade);
                deleteQueue.clear();
                deleteQueueIds.clear();
                deleting = false;
            }
        }

        if (frameId == 0 && deleteQueue.size() == 0)
        {
            deleteQueue = checkDelete(objects, ecs, lengthScale*0.5, fullWidthBinSize);
            deleteQueueIds.clear();
            for (auto o : deleteQueue)
            {
                deleteQueueIds.push_back(o.first);
            }
        }

        if (incoming && !paused)
        {
            elapsed_countdown += double(duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count()-countDownBegin)*1e-9;
            countDownBegin = duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
            if (elapsed_countdown >= countDownSeconds)
            {
                incoming = false;
                console.runString("nextPiece = true");
                elapsed_countdown = 0.0;
            }
        }
    }

    if (!paused && !deleting)
    {
        physics.step(&ecs, &collisions, world.get());
    }
    
    if (!paused && !gameOver)
    {
        if (frameId == 0)
        {
            double unsettlement = energy(objects, ecs) / (1.0+objects.size());
            double threshold = currentSettleThreshold*currentSettleThreshold;

            unsettlement = unsettlement/(threshold*10.0);
            
            int jiggleometer = 10*unsettlement;

            for (int i = 1; i <= 10; i++)
            {
                if (i <= jiggleometer)
                {
                    jiggleometerAlphas[i] = 1.0;
                }
                else
                {
                    jiggleometerAlphas[i] = 0.0;
                }
            }
        }
        else
        {
            for (int i = 1; i <= 10; i++)
            {
                auto id = ecs.idFromHandle("jiggleometer"+std::to_string(i));
                auto & c = ecs.getComponent<cRenderable>(id);
                double diff = c.a-jiggleometerAlphas[i];
                c.a = std::max(std::min(1.0, c.a - (1.0/60.0)*diff),0.0);
            }
        }
    }
}

void to_json(json & j, const JellyCramState & s) {

    std::vector<std::pair<std::string, uint64_t>> deleteQueue;
    std::vector<std::string> deleteQueueIds;
    std::vector<std::string> objects;

    deleteQueue.resize(s.deleteQueue.size());
    deleteQueueIds.resize(s.deleteQueueIds.size());
    objects.resize(s.objects.size());

    std::transform
    (
        s.deleteQueue.begin(),
        s.deleteQueue.end(), 
        deleteQueue.begin(),
        [](std::pair<Id, uint64_t> p) { return std::pair(to_string(p.first), p.second);}
    );

    std::transform
    (
        s.deleteQueueIds.begin(),
        s.deleteQueueIds.end(), 
        deleteQueueIds.begin(),
        [](Id i) { return to_string(i);}
    );

    std::transform
    (
        s.objects.begin(),
        s.objects.end(), 
        objects.begin(),
        [](Id i) { return to_string(i);}
    );

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
        {"currentSettleThreshold", s.currentSettleThreshold},
        {"lengthScale", s.lengthScale},
        {"score", s.score},
        {"events", s.events},
        {"deleteQueue", deleteQueue},
        {"deleteQueueIds", deleteQueueIds},
        {"deleting", s.deleting},
        {"objects", objects},
        {"current", to_string(s.current)}
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
    j.at("currentSettleThreshold").get_to(s.currentSettleThreshold);
    j.at("lengthScale").get_to(s.lengthScale);
    j.at("score").get_to(s.score);
    j.at("events").get_to(s.events);

    std::string sid;
    j.at("current").get_to(sid);
    s.current = Id(sid);

    std::vector<std::pair<std::string, uint64_t>> deleteQueue;
    std::vector<std::string> deleteQueueIds;
    std::vector<std::string> objects;

    j.at("deleteQueue").get_to(deleteQueue);
    j.at("deleteQueueIds").get_to(deleteQueueIds);
    j.at("deleting").get_to(s.deleting);
    j.at("objects").get_to(objects);

    s.deleteQueue.resize(deleteQueue.size());
    s.deleteQueueIds.resize(deleteQueueIds.size());
    s.objects.resize(objects.size());

    std::transform
    (
        deleteQueue.begin(),
        deleteQueue.end(), 
        s.deleteQueue.begin(),
        [](std::pair<std::string, uint64_t> p) { return std::pair(Id(p.first), p.second);}
    );

    std::transform
    (
        deleteQueueIds.begin(),
        deleteQueueIds.end(), 
        s.deleteQueueIds.begin(),
        [](std::string i) { return Id(i);}
    );

    std::transform
    (
        objects.begin(),
        objects.end(), 
        s.objects.begin(),
        [](std::string i) { return Id(i);}
    );
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
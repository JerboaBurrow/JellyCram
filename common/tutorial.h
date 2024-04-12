#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <string>
#include <fstream>

class Tutorial
{

public:
    Tutorial(bool done)
    {
        if (done)
        {
            stage = Stage::DONE;
        }
        else
        {
            stage = Stage::COLLIDE;
            stageComplete = true;
        }
    }

    enum class Stage {Y, X, ROTATE, COLLIDE, JIGGLEOMETER, SMASHER, WEAKER, DONE};

    void next()
    {
        stageComplete = false;
        switch (stage)
        {

        case Stage::COLLIDE:
            stage = Stage::Y;
            break;

        case Stage::Y:
            stage = Stage::X;
            break;

        case Stage::X:
            stage = Stage::ROTATE;
            break;

        case Stage::ROTATE:
            stage = Stage::JIGGLEOMETER;
            stageComplete = true;
            break;

        case Stage::JIGGLEOMETER:
            stage = Stage::SMASHER;
            break;

        case Stage::SMASHER:
            stage = Stage::WEAKER;
            stageComplete = true;
            break;

        case Stage::WEAKER:
            stage = Stage::DONE;
            stageComplete = true;
            break;
        
        default:
            break;
        }
    }

    std::string getTip
    (
        std::string up,
        std::string left,
        std::string rot_left 
    )
    {
        switch (stage)
        {

        case Stage::COLLIDE:
            return collide;

        case Stage::Y:
            return up + " to push the block up";

        case Stage::X:
            return left + " to push the block left";

        case Stage::ROTATE:
            return rot_left + " to rotate anti-clockwise";

        case Stage::JIGGLEOMETER:
            return jiggle;

        case Stage::SMASHER:
            return smash;

        case Stage::WEAKER:
            return weaker;
        
        default:
            return "";
        }
    }

    bool isDone() const { return stage == Stage::DONE;}

    bool isStageComplete() const { return stageComplete; }

    void setStageComplete() { stageComplete = true; }

    Stage getStage() { return stage; }

    void skip() { stage = Stage::DONE; }

    std::string collide = "Collide with the edges or other pieces and you lose control";
    std::string jiggle = "The Jiggleometer must be low for blocks to delete";
    std::string smash = "Use the smasher to break the pieces!";
    std::string weaker = "The controls get weaker the longer you play!";

    bool stageComplete = false;
    
private:

    Stage stage;
        
};

#endif /* TUTORIAL_H */

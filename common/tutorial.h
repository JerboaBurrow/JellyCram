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
            stage = Stage::Y;
        }
    }

    enum class Stage {Y, X, ROTATE, COLLIDE, JIGGLEOMETER, SMASHER, DONE};

    void next()
    {
        switch (stage)
        {
        case Stage::Y:
            stage = Stage::X;
            break;

        case Stage::X:
            stage = Stage::ROTATE;
            break;

        case Stage::ROTATE:
            stage = Stage::COLLIDE;
            break;

        case Stage::COLLIDE:
            stage = Stage::JIGGLEOMETER;
            break;

        case Stage::JIGGLEOMETER:
            stage = Stage::SMASHER;
            break;

        case Stage::SMASHER:
            stage = Stage::DONE;
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
        case Stage::Y:
            return "Press " + up + " to push the block up";

        case Stage::X:
            return "Press " + left + " to push the block left";

        case Stage::ROTATE:
            return "Press " + rot_left + " to rotate anti-clockwise";

        case Stage::COLLIDE:
            return "Collide with the edges or other pieces and you lose control";

        case Stage::JIGGLEOMETER:
            return "The Jiggleometer must be low for blocks to delete";

        case Stage::SMASHER:
            return "Use the smasher to break the pieces!";
        
        default:
            return "";
        }
    }

    bool isDone() const { return stage == Stage::DONE;}

    Stage getStage() { return stage; }

    void skip() { stage = Stage::DONE; }
    
private:

    Stage stage;
        
};

#endif /* TUTORIAL_H */

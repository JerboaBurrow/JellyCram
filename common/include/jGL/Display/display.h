#ifndef DISPLAY_H
#define DISPLAY_H

#include <glm/glm.hpp>

namespace jGL
{
    class Display
    {
    
    public:

        Display(glm::ivec2 res)
        : resolution(res)
        {}

        unsigned getResX() const { return resolution.x; }
        unsigned getResY() const { return resolution.y; }
        glm::ivec2 getRes() const { return resolution; }
        
    protected:

        glm::ivec2 resolution;

    };
}

#endif /* DISPLAY_H */

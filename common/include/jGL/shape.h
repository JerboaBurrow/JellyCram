#ifndef SHAPE_H
#define SHAPE_H

#include <jGL/primitive.h>

namespace jGL
{
    class Shape
    {
    
    public:

        Shape()
        {}

        Shape(Transform tra, glm::vec4 c)
        : transform(tra), colour(c)
        {}

        const Transform & getTransform() const { return transform; }
        const glm::vec4 & getColour() const { return colour; }

        inline virtual void update(Transform tra) { transform = tra; }
        inline virtual void update(glm::vec4 c) { colour = c; }
        inline virtual void update(Transform tra, glm::vec4 c) { transform = tra; colour = c; }

    protected:

        Transform transform;

        glm::vec4 colour;

    };
}

#endif /* SHAPE_H */

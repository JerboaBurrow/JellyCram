#ifndef SHAPERENDERER_H
#define SHAPERENDERER_H

#include <jGL/shape.h>
#include <jGL/shader.h>

#include <unordered_map>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

namespace jGL
{
    typedef std::string ShapeId;

    /*
    
        Looks after shapes, and groups for batch rendering

            Shapes of the same type are instance rendered together
    
    */
    class ShapeRenderer
    {

    public:
    
        ShapeRenderer(size_t sizeHint = 8)
        {
            shapes.reserve(sizeHint); 
        }

        std::shared_ptr<Shape> getShape(ShapeId id);

        const Transform & getTransform(ShapeId id) { return getShape(id)->getTransform(); }
        const glm::vec4 & getColour(ShapeId id) { return getShape(id)->getColour(); }

        virtual void draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, ShapeId> ids) = 0;
        virtual void draw(std::multimap<RenderPriority, ShapeId> ids) = 0;

        virtual void draw() { draw(ids); }
        virtual void draw(std::shared_ptr<Shader> shader) { draw(shader, ids); }

        virtual void draw(std::shared_ptr<Shader> shader, std::vector<ShapeId> ids) = 0;
        virtual void draw(std::vector<ShapeId> ids) = 0;

        virtual void add(std::shared_ptr<Shape> s, ShapeId id, RenderPriority priority = 0);
        
        virtual void remove(ShapeId id)
        {
            if (shapes.find(id) != shapes.end())
            {
                shapes.erase(id);
            }

            for (auto & e : ids)
            {
                if (e.second == id)
                {
                    ids.erase(e.first);
                    break;
                }
            }
        }

        virtual void clear() { ids.clear(); shapes.clear(); }

        bool hasId(const ShapeId id) const { return shapes.find(id) != shapes.end(); } 

        virtual void setProjection(glm::mat4 p) {projection = p;}

        virtual void updatePriority(ShapeId id, RenderPriority newPriority)
        {
            if (shapes.find(id) == shapes.end()){ return; }

            for (auto & e : ids)
            {
                if (e.second == id)
                {
                    ids.erase(e.first);
                    ids.insert(std::pair(newPriority, e.second));
                    break;
                }
            }
        }

    protected:

        std::unordered_map<ShapeId, std::shared_ptr<Shape>> shapes;

        std::multimap<RenderPriority, ShapeId> ids;

        glm::mat4 projection = glm::mat4(0.0f);

    };
}

#endif /* SHAPERENDERER_H */

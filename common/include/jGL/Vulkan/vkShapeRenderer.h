#ifndef VKSHAPERENDERER_H
#define VKSHAPERENDERER_H

#include <jGL/Vulkan/Device/device.h>
#include <jGL/shapeRenderer.h>

namespace jGL::Vulkan
{
    class vkShapeRenderer : public ShapeRenderer
    {
    public:

        vkShapeRenderer(size_t sizeHint)
        : ShapeRenderer(sizeHint)
        {}
        
        void draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, ShapeId> ids){TODO("jGL::Vulkan::vkShape::draw");}
        void draw(std::multimap<RenderPriority, ShapeId> ids) {TODO("jGL::Vulkan::vkShape::draw");}

        void draw(std::shared_ptr<Shader> shader, std::vector<ShapeId> ids) {TODO("jGL::Vulkan::vkShape::draw");}
        void draw(std::vector<ShapeId> ids) {TODO("jGL::Vulkan::vkShape::draw");}

    };
}

#endif /* VKSHAPERENDERER_H */

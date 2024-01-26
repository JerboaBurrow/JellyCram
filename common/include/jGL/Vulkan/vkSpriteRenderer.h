#ifndef VKSPRITERENDERER
#define VKSPRITERENDERER

#include <jGL/Vulkan/Device/device.h>
#include <jGL/spriteRenderer.h>

namespace jGL::Vulkan
{
    class vkSpriteRenderer : public SpriteRenderer
    {
    public:

        vkSpriteRenderer(size_t sizeHint)
        : SpriteRenderer(sizeHint)
        {}
        
        void draw(std::shared_ptr<Shader> shader, std::vector<SpriteId> ids) {TODO("jGL::Vulkan::vkSprite::draw");}
        void draw(std::vector<SpriteId> ids) {TODO("jGL::Vulkan::vkSprite::draw");}

    };
}

#endif /* VKSPRITERENDERER */

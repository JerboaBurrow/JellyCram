#ifndef SPRITERENDERER
#define SPRITERENDERER

#include <jGL/sprite.h>
#include <jGL/shader.h>

#include <unordered_map>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

namespace jGL
{
    typedef std::string SpriteId;

    /*
    
        Looks after sprites, and groups for batch rendering

            If two sprites have the same texture (inc. atlas) they will be stored
                together for batched rendering
    
    */
    class SpriteRenderer
    {

    public:
    
        SpriteRenderer(size_t sizeHint = 8)
        {
            sprites.reserve(sizeHint); 
            textureSlots.reserve(MAX_TEXTURE_SLOTS);
        }

        Sprite & getSprite(SpriteId id);

        const Transform & getTransform(SpriteId id) { return getSprite(id).getTransform(); }
        const TextureOffset & getTextureOffset(SpriteId id) { return getSprite(id).getTextureOffset(); }

        virtual void draw(std::shared_ptr<Shader> shader, std::vector<SpriteId> ids) = 0;
        virtual void draw(std::vector<SpriteId> ids) = 0;
        
        virtual void draw(std::shared_ptr<Shader> shader) { draw(shader, ids); }
        virtual void draw() { draw(ids); }

        virtual void add(Sprite s, SpriteId id);

        virtual void remove(SpriteId id)
        {
            if (sprites.find(id) != sprites.end())
            {
                sprites.erase(id);
            }

            auto iter = std::find(ids.begin(), ids.end(), id);

            if (iter != ids.end())
            {
                ids.erase(iter);
            }
        }

        bool hasId(const SpriteId id) const { return sprites.find(id) != sprites.end(); } 

        virtual void clear() { ids.clear(); sprites.clear(); }

        virtual void setProjection(glm::mat4 p) {projection = p;}

    protected:

        static const uint8_t MAX_TEXTURE_SLOTS = 4;
        std::vector<std::shared_ptr<Texture>> textureSlots;

        uint8_t usedTextureSlots = 0;

        std::unordered_map<SpriteId, Sprite> sprites;

        std::vector<SpriteId> ids;

        glm::mat4 projection = glm::mat4(0.0f);

    };
}

#endif /* SPRITERENDERER */

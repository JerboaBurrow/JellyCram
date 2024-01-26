#ifndef SPRITE_H
#define SPRITE_H

#include <jGL/texture.h>
#include <jGL/primitive.h>

#include <memory>

/*

    A sprite holds a transform (position, rotation, scale)
     and texture offsets (tx, ty, unit)

    The position rotation and scale define where a sprite is drawn

    The texture offsets can be used for atlasing, and selecting a texture
     unit in the shader

*/

namespace jGL
{
    class Sprite
    {
    public:

        Sprite()
        {}

        Sprite
        ( 
            Transform tra,
            TextureOffset to,
            std::shared_ptr<Texture> tex
        )
        : transform(tra), texOffset(to), texture(tex)
        {}

        inline virtual void update(Transform tra) { transform = tra; }
        inline virtual void update(TextureOffset to) { texOffset = to; }
        inline virtual void update(Transform tra, TextureOffset to) { update(tra); update(to); }
        inline virtual void update(std::shared_ptr<Texture> tex) { texture = tex; }
        
        const Transform & getTransform() const { return transform; }
        const TextureOffset & getTextureOffset() const { return texOffset; }
        const Id & getTextureId() const { return texture->getId(); }
        const std::shared_ptr<Texture> & getTexture() const { return texture; }

    protected:

        Transform transform;
        TextureOffset texOffset;
        std::shared_ptr<Texture> texture;
    
    };
}

#endif /* SPRITE_H */

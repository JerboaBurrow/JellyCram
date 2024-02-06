#ifndef GLSPRITERENDERER
#define GLSPRITERENDERER

#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/OpenGL/gl.h>
#include <jGL/spriteRenderer.h>

namespace jGL::GL
{
    class glSpriteRenderer : public SpriteRenderer
    {
    public:

        glSpriteRenderer(size_t sizeHint)
        : SpriteRenderer(sizeHint)
        {
            offsets = std::vector<float>(sizeHint*4+padSprites*4,0.0f);
            textureOffsets = std::vector<float>(sizeHint*3+padSprites*3,0.0f);
            initGL();
            defaultShader = std::make_shared<glShader>(vertexShader, fragmentShader);
            defaultShader->use();
        }

        ~glSpriteRenderer()
        {
            freeGL();
        }

        void draw(std::shared_ptr<Shader> shader, std::multimap<RenderPriority, SpriteId> ids);
        void draw(std::multimap<RenderPriority, SpriteId> ids) { draw(defaultShader, ids); }

        void draw(std::shared_ptr<Shader> shader, std::vector<SpriteId> ids);
        void draw(std::vector<SpriteId> ids) { draw(defaultShader, ids); }
        
    private:

        GLuint vao, a_position, a_offset, a_textureOffset;

        float quad[6*4] = 
        {
            // positions  / texture coords
            0.5f,  0.5f, 1.0f, 1.0f,   // top right
            0.5f,  -0.5f, 1.0f, 0.0f,   // bottom right
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f, 1.0f,    // top left 
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            0.5f,  0.5f, 1.0f, 1.0f  // top right
        };

        std::vector<float> offsets;         // offset x, y, theta, scale
        std::vector<float> textureOffsets;  // tx, ty (atlas coords), texture unit

        size_t padSprites = 8;

        void initGL();
        void freeGL();

        std::shared_ptr<Shader> defaultShader;

        const char * vertexShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float; precision lowp int;\n"
            "layout(location=0) in vec4 a_position;\n"
            "layout(location=1) in vec4 a_offset;\n"
            "layout(location=2) in vec3 a_textureOffset;\n"
            "uniform mat4 proj;\n"
            "out vec2 texCoord;\n"
            "out vec3 texOffset;\n"
            "void main(){"
                "vec2 pos = a_position.xy*a_offset.w;\n"
                "float ct = cos(a_offset.z); float st = sin(a_offset.z);\n"
                "mat2 rot = mat2(ct, -st, st, ct);\n"
                "pos = rot*pos + a_offset.xy;\n"
                "gl_Position = proj*vec4(pos,0.0,1.0);\n"
                "texCoord = a_position.zw;\n"
                "texOffset = a_textureOffset;\n"
            "}";

        const char * fragmentShader = 
            "#version " GLSL_VERSION "\n"
            "precision lowp float; precision lowp int;\n"
            "uniform sampler2D sampler0;\n"
            "uniform sampler2D sampler1;\n"
            "uniform sampler2D sampler2;\n"
            "uniform sampler2D sampler3;\n"
            "in vec2 texCoord;\n"
            "in vec3 texOffset;\n"
            "layout(location=0) out vec4 colour;\n"
            "void main(){\n" 
                // is this mental?
                "if (texOffset.z == 0.0) {colour = texture(sampler0, texCoord);}\n"
                "else if (texOffset.z == 1.0) {colour = texture(sampler1, texCoord);}\n"
                "else if (texOffset.z == 2.0) {colour = texture(sampler2, texCoord);}\n"
                "else if (texOffset.z == 3.0) {colour = texture(sampler3, texCoord);}\n"
                "else {colour = vec4(0.0,0.0,0.0,1.0);}\n"
                ";\n"
            "}";

    };
}

#endif /* GLSPRITERENDERER */

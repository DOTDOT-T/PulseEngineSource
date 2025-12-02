#pragma once
#include "PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.h"
#include "PulseEngine/core/Graphics/TextRenderer.h"
#include <glad.h>
#include <glm/glm.hpp>

#include "stb_truetype.h"

struct TextVert {
    float x, y;
    float u, v;
};

class PULSE_ENGINE_DLL_API GLTextRenderer : public ITextRenderer
{
public:
    ~GLTextRenderer();
    bool Init() override;
    void SetScreenSize(int w, int h) override;

    void RenderText(const std::string& text,
                  float x, float y,
                  float size,
                  const PulseEngine::Vector3& color) override;

    void Render() override;

private:
    struct Vert { float x, y, u, v; };
    struct Quad { Vert v[4]; glm::vec3 color; };

    std::vector<Quad> quads;

    GLuint vao=0, vbo=0, shader=0, fontTex=0;
    int screenW=1, screenH=1;

    static constexpr int ATLAS_W = 512;
    static constexpr int ATLAS_H = 512;

    stbtt_bakedchar bakedChars[96];
    std::vector<unsigned char> ttfBuffer;
    std::vector<unsigned char> atlasBitmap;

private:
    GLuint CompileShader();
};
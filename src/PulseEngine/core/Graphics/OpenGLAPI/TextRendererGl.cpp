#include "TextRendererGl.h"
#include <glad.h>
#include <cstdio>
#include <cstring>

GLTextRenderer::~GLTextRenderer()
{
}

bool GLTextRenderer::Init()
{
    // Load TTF file
    FILE *fp = fopen("Roboto-Regular.ttf", "rb");
    if (!fp)
        return false;

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    ttfBuffer.resize(size);
    fread(ttfBuffer.data(), 1, size, fp);
    fclose(fp);

    // Bake font with stb_truetype
    atlasBitmap.resize(ATLAS_W * ATLAS_H);
    stbtt_BakeFontBitmap(ttfBuffer.data(), 0, 48.0f,
                         atlasBitmap.data(), ATLAS_W, ATLAS_H,
                         32, 96, bakedChars);

    // Create GL texture (GLAD version)
    glGenTextures(1, &fontTex);
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                 ATLAS_W, ATLAS_H, 0,
                 GL_RED, GL_UNSIGNED_BYTE, atlasBitmap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // VAO + VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(TextVert),
                          (void*)offsetof(TextVert, x));

    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          sizeof(TextVert),
                          (void*)offsetof(TextVert, u));

    glBindVertexArray(0);

    shader = CompileShader();

    return true;
}

void GLTextRenderer::SetScreenSize(int w, int h)
{
    screenW = w;
    screenH = h;
}

void GLTextRenderer::RenderText(const std::string &text,
                                float x, float y,
                                float size,
                                const PulseEngine::Vector3 &color)
{
    float xpos = x;
    float ypos = y;

    for (char c : text)
    {
        if (c < 32 || c >= 128) continue;

        stbtt_bakedchar *b = &bakedChars[c - 32];

        float x0 = xpos + b->xoff * (size / 48.0f);
        float y0 = ypos + b->yoff * (size / 48.0f); // <- inversé
        float x1 = x0 + (b->x1 - b->x0) * (size / 48.0f);
        float y1 = y0 + (b->y1 - b->y0) * (size / 48.0f); // <- inversé

        float s0 = b->x0 / float(ATLAS_W);
        float t0 = b->y0 / float(ATLAS_H);
        float s1 = b->x1 / float(ATLAS_W);
        float t1 = b->y1 / float(ATLAS_H);

        Quad q;
        q.color = glm::vec3(color.x, color.y, color.z);

        q.v[0] = { x0, y0, s0, t0 };
        q.v[1] = { x1, y0, s1, t0 };
        q.v[2] = { x1, y1, s1, t1 };
        q.v[3] = { x0, y1, s0, t1 };

        quads.push_back(q);

        xpos += b->xadvance * (size / 48.0f); // <- correction
    }
}


void GLTextRenderer::Render()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader);

    glUniform2f(glGetUniformLocation(shader, "uScreen"),
                (float)screenW, (float)screenH);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTex);

    for (const auto &q : quads)
    {
        // Push quad into VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(q.v), q.v, GL_DYNAMIC_DRAW);

        glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, &q.color.x);
        glUniform1i(glGetUniformLocation(shader, "uTex"), 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    quads.clear();
}

GLuint GLTextRenderer::CompileShader()
{
    const char *vs = R"(
        #version 330 core
        layout(location=0) in vec2 aPos;
        layout(location=1) in vec2 aUV;

        uniform vec2 uScreen;
        out vec2 vUV;

        void main()
        {
            vec2 pos = (aPos / uScreen) * 2.0 - 1.0;
            pos.y = -pos.y;
            gl_Position = vec4(pos, 0.0, 1.0);
            vUV = aUV;
        }
    )";

    const char *fs = R"(
        #version 330 core
        in vec2 vUV;
        out vec4 FragColor;

        uniform sampler2D uTex;
        uniform vec3 uColor;

        void main()
        {
            float a = texture(uTex, vUV).r;
            FragColor = vec4(uColor, a);
        }
    )";

    auto compile = [&](GLenum type, const char *src)
    {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        return s;
    };

    GLuint vsId = compile(GL_VERTEX_SHADER, vs);
    GLuint fsId = compile(GL_FRAGMENT_SHADER, fs);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vsId);
    glAttachShader(prog, fsId);
    glLinkProgram(prog);

    glDeleteShader(vsId);
    glDeleteShader(fsId);

    return prog;
}

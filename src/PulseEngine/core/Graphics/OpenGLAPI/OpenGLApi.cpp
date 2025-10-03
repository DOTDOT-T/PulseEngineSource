#include "PulseEngine/core/WindowContext/WindowContext.h"
#include "PulseEngine/core/Graphics/OpenGLAPI/OpenGLApi.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "OpenGLApi.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 
#include "PulseEngine/core/Meshes/Mesh.h"

bool OpenGLAPI::InitializeApi(const char* title, int* width, int* height, PulseEngineBackend* engine)
{
    std::cout << "starting to init opengl" << std::endl;
        this->engine = engine;
    if (!glfwInit())
    {
        EDITOR_ERROR("Failed to initialize GLFW.");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    this->width = width;
    this->height = height;

    window = glfwCreateWindow(*width, *height, title, nullptr, nullptr);
    if (!window)
    {
        EDITOR_ERROR("Failed to create GLFW window.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        EDITOR_ERROR("Error while initializing GLAD.");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    glfwMakeContextCurrent(window);
    glEnable(GL_MULTISAMPLE);

        // Create texture to render to
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    // Create Renderbuffer for depth and stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        EDITOR_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cout << "OpenGL API initialized successfully." << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    return true;
}

void OpenGLAPI::ShutdownApi()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void OpenGLAPI::UseShader(unsigned int shaderID) const
{
    glUseProgram(shaderID);
}

void OpenGLAPI::SetShaderMat4(const Shader* shader, const std::string &name, const PulseEngine::Mat4 &mat) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    glm::mat4 matrix = glm::mat4(
        mat[0][0], mat[0][1], mat[0][2], mat[0][3],
        mat[1][0], mat[1][1], mat[1][2], mat[1][3],
        mat[2][0], mat[2][1], mat[2][2], mat[2][3],
        mat[3][0], mat[3][1], mat[3][2], mat[3][3]
    );

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLAPI::SetShaderMat3(const Shader *shader, const std::string &name, const PulseEngine::Mat3 &mat) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    glm::mat3 matrix = glm::mat3(
        mat[0][0], mat[0][1], mat[0][2],
        mat[1][0], mat[1][1], mat[1][2],
        mat[2][0], mat[2][1], mat[2][2]
    );

    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLAPI::SetShaderVec3(const Shader* shader, const std::string &name, const PulseEngine::Vector3 &vec) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    glm::vec3 glVec(vec.x, vec.y, vec.z);
    glUniform3fv(location, 1, glm::value_ptr(glVec));
}
void OpenGLAPI::SetShaderFloat(const Shader *shader, const std::string &name, float value) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    glUniform1f(location, value);
}
void OpenGLAPI::SetShaderBool(const Shader *shader, const std::string &name, bool value) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    glUniform1i(location, static_cast<int>(value));
}
void OpenGLAPI::SetShaderInt(const Shader *shader, const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(shader->getProgramID(), name.c_str()), value);
}

void OpenGLAPI::SetShaderIntArray(const Shader *shader, const std::string &name, const int *values, int count) const
{    
    glUniform1iv(glGetUniformLocation(shader->getProgramID(), name.c_str()), count, values);
}

void OpenGLAPI::SetShaderVec3Array(const Shader *shader, const std::string &name, const std::vector<PulseEngine::Vector3> &vecArray) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    if (location == -1 || vecArray.empty())
        return;

    std::vector<glm::vec3> glmVecs;
    glmVecs.reserve(vecArray.size());
    for (const auto& v : vecArray)
    {
        glmVecs.emplace_back(v.x, v.y, v.z);
    }

    glUniform3fv(location, static_cast<GLsizei>(glmVecs.size()), glm::value_ptr(glmVecs[0]));
}

void OpenGLAPI::SetShaderFloatArray(const Shader *shader, const std::string &name, const std::vector<float> &floatArray) const
{
    int location = glGetUniformLocation(shader->getProgramID(), name.c_str());
    if (location == -1 || floatArray.empty())
        return;

    glUniform1fv(location, static_cast<GLsizei>(floatArray.size()), floatArray.data());
}

void OpenGLAPI::ActivateTexture(unsigned int textureID) const
{
    glActiveTexture(GL_TEXTURE0 + textureID);
    
}

void OpenGLAPI::BindTexture(TextureType type, unsigned int textureID) const
{
    switch (type)
    {
        case TEXTURE_2D:
            glBindTexture(GL_TEXTURE_2D, textureID);
            break;
        case TEXTURE_CUBE_MAP:
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        default:
            EDITOR_ERROR("Unknown texture type.");
            break;
    }
}

void OpenGLAPI::GenerateDepthCubeMap(unsigned int *FBO, unsigned int *depthCubeMap) const
{    
    glGenFramebuffers(1, FBO);

    glGenTextures(1, depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubeMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
                     DEFAULT_SHADOW_MAP_RES, DEFAULT_SHADOW_MAP_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "PointLight: Initial framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool OpenGLAPI::IsFrameBufferComplete() const
{
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void OpenGLAPI::InitCubeMapFaceForRender(unsigned int *CubeMap, unsigned int faceIndex) const
{
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, *CubeMap, 0);
}

void OpenGLAPI::GenerateTextureMap(unsigned int *textureID, const std::string &filePath) const
{
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);

    // Paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // Flip vertical selon besoin
    unsigned char* data = stbi_load((std::string(ASSET_PATH) + filePath).c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        EDITOR_ERROR("Failed to load texture: " + (std::string(ASSET_PATH) + filePath));
    }

    stbi_image_free(data);
}

void OpenGLAPI::GenerateShadowMap(unsigned int *shadowMap, unsigned int *FBO, int width, int height) const
{
    glGenFramebuffers(1, FBO);

    glGenTextures(1, shadowMap);
    glBindTexture(GL_TEXTURE_2D, *shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

    glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *shadowMap, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        EDITOR_ERROR("Shadow map framebuffer is not complete!");
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLAPI::BindShadowFramebuffer(unsigned int *FBO) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
    glViewport(0, 0, DEFAULT_SHADOW_MAP_RES, DEFAULT_SHADOW_MAP_RES);
    glClear(GL_DEPTH_BUFFER_BIT);
     glCullFace(GL_FRONT); 
}

void OpenGLAPI::UnbindShadowFramebuffer() const
{
     glCullFace(GL_BACK); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLAPI::SetupSimpleSquare(unsigned int* VAO, unsigned int* VBO , unsigned int* EBO) const
{
    float vertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f  // top right
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void OpenGLAPI::DeleteMesh(unsigned int* VAO, unsigned int* VBO) const
{
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
}

void OpenGLAPI::SetupMesh(unsigned int *VAO, unsigned int *VBO, unsigned int* EBO, const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) const
{
        glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);

    // Envoie des données des sommets
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Envoie des indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribut 0 : Position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

    // Attribut 1 : Normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Attribut 2 : BoneIDs (ivec4) → glVertexAttribIPointer !
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    // Attribut 3 : Weights (vec4)
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

    // Attribut 4 : TexCoords (vec2)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void OpenGLAPI::RenderMesh(unsigned int *VAO, unsigned int *VBO, const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) const
{
     glBindVertexArray(*VAO);

    if (!indices.empty())
    {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    }

    glBindVertexArray(0);
}

float OpenGLAPI::GetTime() const
{
    return glfwGetTime();
}

void OpenGLAPI::FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// void OpenGLAPI::PollEvents() const
// {
//     glfwSetCursorPosCallback(window, MouseInput::MouseCallback);
//     // glfwSetScrollCallback(window, MouseInput::ScrollCallback);

//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
// }

void OpenGLAPI::SwapBuffers() const
{
    glfwSwapBuffers(window);
}

bool OpenGLAPI::ShouldClose() const
{
    if (!window)
    {
        EDITOR_ERROR("GLFW window is not initialized.");
        return true; // Consider it closed if window is null
    }
    std::cout << glfwWindowShouldClose(window) << std::endl;
    
    return glfwWindowShouldClose(window);
}

void OpenGLAPI::SetWindowSize(int width, int height) const
{
    glfwSetWindowSize(window, width, height);
}

void OpenGLAPI::SetWindowTitle(const char *title) const
{
    glfwSetWindowTitle(window, title);
}

void *OpenGLAPI::GetNativeHandle() const
{
    return static_cast<void*>(window);
}

void OpenGLAPI::PollEvents() const
{
        glfwPollEvents();    
}

void OpenGLAPI::StartFrame() const
{
    EDITOR_ONLY(
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, fboWidth, fboHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    )
    IN_GAME_ONLY(
        glfwGetFramebufferSize(window, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, *width, *height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.8f, 0.8f, 0.6f, 1.0f);
    )
}

void OpenGLAPI::SpecificStartFrame(int specificVBO, const PulseEngine::Vector2& frameSize) const
{        
    glBindFramebuffer(GL_FRAMEBUFFER, specificVBO);
    glViewport(0, 0, frameSize.x, frameSize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void OpenGLAPI::EndFrame() const
{
    EDITOR_ONLY(
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Go back to default framebuffer
        glViewport(0, 0, 1920, 1080); // Reset to default screen size
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    )
}

void OpenGLAPI::ActivateBackCull() const
{
    glCullFace(GL_BACK); 
}

unsigned int OpenGLAPI::CreateShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Charger et compiler les shaders
    std::string vertexCode = LoadShaderCode(vertexPath);
    std::string fragmentCode = LoadShaderCode(fragmentPath);
    
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());
    unsigned int shaderID = LinkProgram(vertexShader, fragmentShader);
    return shaderID;
}

unsigned int OpenGLAPI::CreateShader(const std::string &vertexPath,
                                     const std::string &fragmentPath,
                                     const std::string &geometryPath)
{
    // Load shader source
    std::string vertexCode   = LoadShaderCode(vertexPath);
    std::string fragmentCode = LoadShaderCode(fragmentPath);

    unsigned int vertexShader   = CompileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    unsigned int geometryShader = 0;
    if (!geometryPath.empty())
    {
        std::string geometryCode = LoadShaderCode(geometryPath);
        geometryShader = CompileShader(GL_GEOMETRY_SHADER, geometryCode.c_str());
    }

    // Link program
    unsigned int shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    if (geometryShader != 0)
        glAttachShader(shaderID, geometryShader);
    glLinkProgram(shaderID);

    // Check linking errors (you can expand your LinkProgram function for this)
    int success;
    char infoLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    return shaderID;
}


std::string OpenGLAPI::LoadShaderCode(const std::string& path)
{
    std::ifstream shaderFile(path);
    if (!shaderFile.is_open())
    {
        std::cerr << "Erreur : impossible d'ouvrir le fichier shader : " << path << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

unsigned int OpenGLAPI::CompileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Erreur de compilation du "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader:\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int OpenGLAPI::LinkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Erreur de linkage du shader program:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


OpenGLAPI::~OpenGLAPI()
{
    Shutdown();
}
extern "C" __declspec(dllexport) IModule* CreateModule()
{
    return new OpenGLAPI();
}


// graphicsAPI.cpp
__declspec(dllexport) void InitializeGraphics()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}
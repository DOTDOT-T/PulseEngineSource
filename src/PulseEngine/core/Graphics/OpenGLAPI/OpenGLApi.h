/**
 * @file OpenGLApi.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief implementation of the OpenGL graphics API for Pulse Engine.
 * @brief This class implements the IGraphicsAPI interface and provides methods to initialize, shutdown...
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include <glad.h>                       // OpenGL function loader
#include <GLFW/glfw3.h>                 // Cross-platform windowing/input
#include <glm/glm.hpp>                  // GLM core
#include <glm/gtc/type_ptr.hpp>         // GLM pointer helpers
#include <glm/gtc/matrix_transform.hpp> // GLM transformation helpers
#include <GL/gl.h>                      // OpenGL headers

class GLTextRenderer;

class OpenGLAPI : public IGraphicsAPI
{
public:
    OpenGLAPI() = default;
    virtual ~OpenGLAPI() override;

    bool InitializeApi(const char* title, int* width, int* height) override;
    void ShutdownApi() override;

    void PollEvents() const override;
    void SwapBuffers() const override;
    bool ShouldClose() const override;

    void DrawGridQuad(PulseEngine::Mat4 viewCam,const PulseEngine::Mat4& specificProjection , IGraphicsAPI* graphicsAPI) override;
    void DrawLine(const PulseEngine::Vector3& start, const PulseEngine::Vector3& end, const PulseEngine::Color& color) override;
    
    void SetWindowSize(int width, int height) const override;
    void SetWindowTitle(const char* title) const override;

    void* GetNativeHandle() const override;
    std::string GetApiName() const override { return "OpenGL"; }

        unsigned int CreateShader(const std::string& vertexPath, const std::string& fragmentPath) override;
        unsigned int CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) override;
    std::string LoadShaderCode(const std::string& path);
    unsigned int CompileShader(unsigned int type, const char* source);
    unsigned int LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);

    void UseShader(unsigned int shaderID) const override;
    void SetShaderMat4(const Shader* shader, const std::string& name, const PulseEngine::Mat4& mat) const override;
    void SetShaderMat3(const Shader* shader, const std::string& name, const PulseEngine::Mat3& mat) const override;
    void SetShaderVec3(const Shader* shader, const std::string& name, const PulseEngine::Vector3& vec) const override;
    void SetShaderFloat(const Shader* shader, const std::string& name, float value) const override;
    void SetShaderBool(const Shader* shader, const std::string& name, bool value) const override;
    void SetShaderInt(const Shader* shader, const std::string& name, int value) const override;
    void SetShaderIntArray(const Shader* shader, const std::string& name, const int* values, int count) const override;
    void SetShaderVec3Array(const Shader* shader, const std::string& name, const std::vector<PulseEngine::Vector3>& vecArray) const override;
    void SetShaderFloatArray(const Shader* shader, const std::string& name, const std::vector<float>& floatArray) const override;
    void SetShaderMat4Array(const Shader* shader, const std::string& name, const std::vector<PulseEngine::Mat4>& array) const override;
    void ActivateTexture(unsigned int textureID) const override;
    void BindTexture(TextureType type, unsigned int textureID) const override;

    void GenerateDepthCubeMap(unsigned int* FBO, unsigned int* depthCubeMap) const override;
    bool IsFrameBufferComplete() const override;
    void InitCubeMapFaceForRender(unsigned int* CubeMap, unsigned int faceIndex) const override;
    void GenerateTextureMap(unsigned int* textureID, const std::string& filePath, bool hasFlip) const override;
    void GenerateShadowMap(unsigned int* shadowMap, unsigned int* FBO, int width, int height) const override;
    void BindShadowFramebuffer(unsigned int* FBO) const override;
    void UnbindShadowFramebuffer() const override;

    void SetupSimpleSquare(unsigned int* VAO, unsigned int* VBO , unsigned int* EBO) const override;

    void DeleteMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) const override;
    void SetupMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) const override;
    void RenderMesh(unsigned int* VAO, unsigned int* VBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) const override;

    float GetTime() const override;
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    void StartFrame() const override;
    void SpecificStartFrame(int specificVBO, const PulseEngine::Vector2& frameSize) const override;

    void EndFrame(bool onlyUnbind) const override;
    void ActivateBackCull() const override;

    void GenerateFrameBuffer(unsigned int* previewFBO, unsigned int* previewTexture, unsigned int* rbo, unsigned int previewWidth,unsigned int previewHeight) override;
    void RenderLineMesh(unsigned int* VAO, unsigned int* VBO, const std::vector<PulseEngine::Vector3>& vertices, const std::vector<unsigned int>& indices) override;
    void ActivateWireframe() override;
    void DesactivateWireframe() override;

    
    ITextRenderer* CreateNewText() override;

    GLFWwindow* window = nullptr;
    GLuint fbo, fboTexture, rbo;
    int fboWidth = 1024;
    int fboHeight = 720;
private:
};

#endif // OPENGLAPI_H

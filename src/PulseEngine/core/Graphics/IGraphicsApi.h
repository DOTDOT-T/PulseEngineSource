/**
 * @file IGraphicsApi.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief The most important part of the Pulse Engine, the graphics API interface.
 * @brief This interface defines all the necessary methods required to initialize, manage, and use a rendering backend for the Pulse engine.
 * @brief It is used by the PulseEngineBackend to communicate with the graphics API.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef IGRAPHICSAPI_H
#define IGRAPHICSAPI_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include <string>

#include "PulseEngine/ModuleLoader/IModule/IModule.h"
#include "PulseEngine/core/Meshes/Mesh.h"



class PulseEngineBackend;

enum TextureType
{
    TEXTURE_2D,
    TEXTURE_CUBE_MAP
};

/**
 * @brief Interface for any graphics API backend (OpenGL, Vulkan, DirectX, etc.).
 * 
 * This abstract class defines all the necessary methods required to initialize,
 * manage, and use a rendering backend for the Pulse engine.
 */
class PULSE_ENGINE_DLL_API IGraphicsAPI : public IModule
{
public:
    virtual ~IGraphicsAPI() = default;

    virtual std::string GetName() const override { return "IGraphicsAPI"; }
    virtual std::string GetVersion() const override { return "0.1"; }
    virtual void Initialize() override {}
    virtual void Shutdown() override {}

    // ===== Initialization & Shutdown =====

    /**
     * @brief Initializes the graphics API and creates the main window.
     * 
     * @param title The window title.
     * @param width Pointer to the desired window width (can be modified).
     * @param height Pointer to the desired window height (can be modified).
     * @param engine Pointer to the Pulse engine backend instance.
     * @return true if initialization succeeds, false otherwise.
     */
    virtual bool InitializeApi(const char* title, int* width, int* height, PulseEngineBackend* engine) = 0;

    /**
     * @brief Shuts down the graphics API and releases all resources.
     */
    virtual void ShutdownApi() = 0;

    // ===== Window Management =====

    /**
     * @brief Polls input and window events (e.g., key presses, window resize).
     */
    virtual void PollEvents() const = 0;

    /**
     * @brief Swaps the front and back buffer (usually called once per frame).
     */
    virtual void SwapBuffers() const = 0;

    /**
     * @brief Checks if the window should close.
     * 
     * @return true if the window is closing, false otherwise.
     */
    virtual bool ShouldClose() const = 0;

    /**
     * @brief Sets the window size.
     * 
     * @param width The new width.
     * @param height The new height.
     */
    virtual void SetWindowSize(int width, int height) const = 0;

    /**
     * @brief Sets the window title.
     * 
     * @param title The new window title.
     */
    virtual void SetWindowTitle(const char* title) const = 0;

    /**
     * @brief Returns a handle to the native window (e.g., GLFWwindow*).
     * 
     * @return A void pointer to the native window.
     */
    virtual void* GetNativeHandle() const = 0;

    /**
     * @brief Gets the name of the current graphics API (e.g., "OpenGL", "Vulkan").
     * 
     * @return A string representing the API name.
     */
    virtual std::string GetApiName() const = 0;

    // ===== Rendering Flow =====

    /**
     * @brief Starts a new rendering frame. Should clear buffers and prepare for drawing.
     */
    virtual void StartFrame() const = 0;

    virtual void SpecificStartFrame(int specificVBO, const PulseEngine::Vector2& frameSize) const = 0;

    /**
     * @brief Ends the rendering frame. Usually includes buffer swapping or command submission.
     */
    virtual void EndFrame() const = 0;
    virtual void ActivateBackCull() const = 0;

    virtual unsigned int CreateShader(const std::string& vertexPath, const std::string& fragmentPath) = 0;
    virtual unsigned int CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) = 0;

    // ===== Shader Management =====
    virtual void UseShader(unsigned int shaderID) const = 0;
    virtual void SetShaderMat4(const Shader* shader, const std::string& name, const PulseEngine::Mat4& mat) const = 0;
    virtual void SetShaderMat3(const Shader* shader, const std::string& name, const PulseEngine::Mat3& mat) const = 0;
    virtual void SetShaderVec3(const Shader* shader, const std::string& name, const PulseEngine::Vector3& vec) const = 0;
    virtual void SetShaderFloat(const Shader* shader, const std::string& name, float value) const = 0;
    virtual void SetShaderBool(const Shader* shader, const std::string& name, bool value) const = 0;
    virtual void SetShaderInt(const Shader* shader, const std::string& name, int value) const = 0;
    virtual void SetShaderIntArray(const Shader* shader, const std::string& name, const int* values, int count) const = 0;
    virtual void SetShaderVec3Array(const Shader* shader, const std::string& name, const std::vector<PulseEngine::Vector3>& vecArray) const = 0;
    virtual void SetShaderFloatArray(const Shader* shader, const std::string& name, const std::vector<float>& floatArray) const = 0;

    // ===== Texture and map =====
    virtual void GenerateDepthCubeMap(unsigned int* FBO, unsigned int* depthCubeMap) const = 0;
    virtual bool IsFrameBufferComplete() const = 0;
    virtual void InitCubeMapFaceForRender(unsigned int* CubeMap, unsigned int faceIndex) const = 0;
    virtual void GenerateTextureMap(unsigned int* textureID, const std::string& filePath) const = 0;
    virtual void GenerateShadowMap(unsigned int* shadowMap, unsigned int* FBO, int width, int height) const = 0;
    virtual void BindShadowFramebuffer(unsigned int* FBO) const = 0;
    virtual void UnbindShadowFramebuffer() const = 0;

    // ===== Mesh and vertex =====
    virtual void DeleteMesh(unsigned int* VAO, unsigned int* VBO) const = 0;

    virtual void SetupSimpleSquare(unsigned int* VAO, unsigned int* VBO , unsigned int* EBO) const = 0;


    virtual float GetTime() const = 0; 

    // ===== Texture management =====
    virtual void ActivateTexture(unsigned int textureID) const = 0;
    virtual void BindTexture(TextureType type, unsigned int textureID) const = 0;
    virtual void SetupMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) const = 0;
    virtual void RenderMesh(unsigned int* VAO, unsigned int* VBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) const = 0;

    // ===== Shared Context Variables =====

    /// Pointer to current window width.
    int* width = nullptr;

    /// Pointer to current window height.
    int* height = nullptr;

    /// Pointer to the engine backend, for API-engine communication.
    PulseEngineBackend* engine = nullptr;
};

#endif // IGRAPHICSAPI_H

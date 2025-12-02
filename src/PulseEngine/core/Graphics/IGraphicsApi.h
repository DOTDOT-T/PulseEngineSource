/**
 * @file IGraphicsApi.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
* @brief The central abstraction layer of the Pulse Engine rendering system.
 *
 * @details
 * IGraphicsAPI defines the unified graphics backend interface used by the Pulse Engine.
 * It provides an abstraction over specific rendering APIs (OpenGL, Vulkan, DirectX, etc.)
 * and exposes a consistent contract for rendering, shader management, texture operations,
 * and frame orchestration.
 * 
 * This interface is the backbone of the rendering architecture — it ensures that the
 * engine and editor can operate independently of any specific graphics backend.
 *
 * @section Architecture Architecture Overview
 * 
 * ┌──────────────────────────┐
 * │      Pulse Engine        │
 * │  (Scene, ECS, Renderer)  │
 * └───────────┬──────────────┘
 *             │
 *             ▼
 *   ┌────────────────────┐
 *   │   IGraphicsAPI     │  ← Abstract interface (this file)
 *   └────────────────────┘
 *             │
 *   ┌─────────┴──────────┐  ┌───────────────────┐ ┌───────────────────┐
 *   │   OpenGLBackend    │  │   VulkanBackend   │ │  DirectXBackend   │  
 *   └────────────────────┘  └───────────────────┘ └───────────────────┘
 * 
 * Each backend implements this interface and registers itself through
 * the PulseEngineBackend during engine initialization.
 *
 * @section Responsibilities
 * - Abstract rendering API calls.
 * - Manage GPU resources and synchronization.
 * - Provide frame lifecycle control (StartFrame/EndFrame).
 * - Act as the bridge between engine logic and graphics drivers.
 *
 * @section Extension Adding a New Backend
 * To implement a new backend:
 *  1. Create a subclass inheriting from IGraphicsAPI.
 *  2. Implement all pure virtual methods.
 *  3. Register the backend in the PulseEngineBackend factory.
 *
 * @warning This interface must remain API-stable. Breaking changes
 * will affect all rendering backends.
 * @version 0.2
 * @date 2025-10-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef IGRAPHICSAPI_H
#define IGRAPHICSAPI_H

// #include "Common/common.h"
#include "Common/dllExport.h"
#include <string>
#include <vector>

#include "PulseEngine/ModuleLoader/IModule/IModule.h"
#include "PulseEngine/core/Math/Color.h"

#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Math/Mat4.h"
#include "PulseEngine/core/Math/Mat3.h"
#include "shader.h"



class PulseEngineBackend;
class Vertex;
class ITextRenderer;

/**
 * @enum TextureType
 * @brief Defines texture binding targets used by rendering backends.
 */
enum TextureType
{
    TEXTURE_2D,
    TEXTURE_CUBE_MAP
};

/**
 * @class IGraphicsAPI
 * @brief Core rendering interface of the Pulse Engine.
 *
 * @details
 * Defines the abstract contract that all graphics backends must implement.
 * This includes initialization, window handling, frame management,
 * shader and texture control, and mesh rendering.
 *
 * @note
 * All concrete implementations (e.g., OpenGLAPI, VulkanAPI)
 * should inherit from this class and override every pure virtual method.
 *
 * @see PulseEngineBackend
 */
class PULSE_ENGINE_DLL_API IGraphicsAPI : public IModule
{
public:
    virtual ~IGraphicsAPI() = default;

    virtual std::string GetName() const override { return "IGraphicsAPI"; }
    virtual std::string GetVersion() const override { return "0.1"; }

    // ============================================================================
    //  Initialization & Lifecycle
    // ============================================================================
    // These methods handle API setup, teardown, and synchronization with the
    // PulseEngineBackend. Initialization should create the rendering context,
    // main window, and essential GPU states.

    virtual void Initialize() override {}
    
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
    virtual bool InitializeApi(const char* title, int* width, int* height) = 0;
    virtual void Shutdown() override {}

    virtual void DrawGridQuad(PulseEngine::Mat4 viewCam,const PulseEngine::Mat4& specificProjection , IGraphicsAPI* graphicsAPI) = 0;
    virtual void DrawLine(const PulseEngine::Vector3& start, const PulseEngine::Vector3& end, const PulseEngine::Color& color) = 0;

    
    virtual void ActivateWireframe() = 0;
    virtual void DesactivateWireframe() = 0;

    /**
     * @brief Shuts down the graphics API and releases all resources.
     */
    virtual void ShutdownApi() = 0;


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
    virtual void EndFrame(bool onlyUnbind = false) const = 0;
    virtual void ActivateBackCull() const = 0;

    virtual unsigned int CreateShader(const std::string& vertexPath, const std::string& fragmentPath) = 0;
    virtual unsigned int CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) = 0;

    virtual void GenerateFrameBuffer(unsigned int* previewFBO, unsigned int* previewTexture, unsigned int* rbo, unsigned int previewWidth,unsigned int previewHeight) = 0;

    // ============================================================================
    //  Shader Management
    // ============================================================================
    // Abstracts shader compilation and parameter binding.
    // Every backend must ensure shader uniform consistency with the engine's
    // material and render pipeline definitions.

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
    virtual void SetShaderMat4Array(const Shader* shader, const std::string& name, const std::vector<PulseEngine::Mat4>& array) const = 0;

    // ============================================================================
    //  Texture & Shadow Mapping
    // ============================================================================
    // Responsible for GPU texture creation, shadow map management, and cube map
    // initialization. Backends must support depth-based rendering pipelines.
    virtual void GenerateDepthCubeMap(unsigned int* FBO, unsigned int* depthCubeMap) const = 0;
    virtual bool IsFrameBufferComplete() const = 0;
    virtual void InitCubeMapFaceForRender(unsigned int* CubeMap, unsigned int faceIndex) const = 0;
    virtual void GenerateTextureMap(unsigned int* textureID, const std::string& filePath) const = 0;
    virtual void GenerateShadowMap(unsigned int* shadowMap, unsigned int* FBO, int width, int height) const = 0;
    virtual void BindShadowFramebuffer(unsigned int* FBO) const = 0;
    virtual void UnbindShadowFramebuffer() const = 0;
    virtual void ActivateTexture(unsigned int textureID) const = 0;
    virtual void BindTexture(TextureType type, unsigned int textureID) const = 0;
    virtual void SetupMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) const = 0;
    virtual void RenderMesh(unsigned int* VAO, unsigned int* VBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) const = 0;

    virtual void RenderLineMesh(unsigned int* VAO, unsigned int* VBO, const std::vector<PulseEngine::Vector3>& vertices, const std::vector<unsigned int>& indices) = 0;

    // ============================================================================
    //  Mesh & Vertex Management
    // ============================================================================
    // Defines vertex array, buffer, and index setup routines.
    // Used by the engine renderer to stream geometry to the GPU.
    virtual void DeleteMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) const = 0;

    virtual void SetupSimpleSquare(unsigned int* VAO, unsigned int* VBO , unsigned int* EBO) const = 0;


    virtual float GetTime() const = 0; 

    // ============================================================================
    // TEXT MANAGER
    // ============================================================================
    virtual ITextRenderer* CreateNewText() = 0;


    // ===== Shared Context Variables =====

    /// Pointer to current window width.
    int* width = nullptr;

    /// Pointer to current window height.
    int* height = nullptr;
};

#endif // IGRAPHICSAPI_H

// File: GLRenderDeviceAdapter.h
#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "IRenderDevice.h" // engine-facing interface (from skeleton)

    // --- Vertex data ---
    struct Vertex {
        float pos[3];
        float color[3];
    };
// Forward declarations
class GLCommandList;

class GLRenderDeviceAdapter : public IRenderDevice
{
public:
    GLRenderDeviceAdapter();
    virtual ~GLRenderDeviceAdapter();

    // IRenderDevice
    bool Initialize(const char* appName, int width, int height) override;
    void Shutdown() override;
    void Resize(uint32_t width, uint32_t height) override;

    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;

    BufferHandle CreateBuffer(const GpuBufferDesc& desc, const void* initialData = nullptr) override;
    void DestroyBuffer(BufferHandle h) override;

    TextureHandle CreateTexture(const GpuTextureDesc& desc) override;
    void DestroyTexture(TextureHandle h) override;

    PipelineHandle CreatePipeline(const PipelineDesc& desc) override;
    void DestroyPipeline(PipelineHandle h) override;

    DescriptorLayoutHandle CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) override;
    DescriptorSetHandle AllocateDescriptorSet(DescriptorLayoutHandle layout) override;
    void UpdateDescriptorSet(DescriptorSetHandle set, const DescriptorSetDesc& desc) override;

    CommandListHandle CreateCommandList() override;
    ICommandList* GetCommandList(CommandListHandle) override;
    CommandQueueHandle GetGraphicsQueue() override;

    void Submit(CommandQueueHandle queue, CommandListHandle list, FenceHandle fenceToSignal = 0) override;

    void WaitForFence(FenceHandle fence, uint64_t value) override;

    float GetTimeSeconds() const override;
    
    uint32_t GetBufferId(BufferHandle h) const override; 

    VertexArrayHandle CreateVertexArray(const VertexArrayDesc& desc) override;
    void DestroyVertexArray(VertexArrayHandle h) override;
    void BindVertexArray(VertexArrayHandle h) override;
private:
    // GLFW window
    GLFWwindow* m_window = nullptr;
    uint32_t m_width = 1280;
    uint32_t m_height = 720;

    // Simple handle allocators
    uint32_t m_nextBufferHandle = 1;
    uint32_t m_nextTextureHandle = 1;
    uint32_t m_nextPipelineHandle = 1;
    uint32_t m_nextDescriptorLayoutHandle = 1;
    uint32_t m_nextDescriptorSetHandle = 1;
    uint32_t m_nextCommandListHandle = 1;
    uint32_t m_nextVAOHandle = 1;

    struct BufferEntry { GLuint glId; GpuBufferDesc desc; };
    struct TextureEntry { GLuint glId; GpuTextureDesc desc; };
    struct PipelineEntry { PipelineDesc desc; GLuint program = 0; };
    struct DescriptorLayoutEntry { DescriptorSetLayoutDesc desc; };
    struct DescriptorSetEntry { DescriptorSetDesc desc; };

    std::unordered_map<BufferHandle, BufferEntry> m_buffers;
    std::unordered_map<TextureHandle, TextureEntry> m_textures;
    std::unordered_map<PipelineHandle, PipelineEntry> m_pipelines;
    std::unordered_map<DescriptorLayoutHandle, DescriptorLayoutEntry> m_layouts;
    std::unordered_map<DescriptorSetHandle, DescriptorSetEntry> m_sets;
    std::unordered_map<VertexArrayHandle, GLuint> m_vaos;

    std::unordered_map<CommandListHandle, std::unique_ptr<GLCommandList>> m_commandLists;

    // Single graphics queue emulated as 0
    CommandQueueHandle m_graphicsQueue = 0;

    double m_startTime = 0.0;

    // Helpers
    GLuint CreateGLBuffer(const GpuBufferDesc& desc, const void* data);
    GLuint CreateGLTexture(const GpuTextureDesc& desc);
    GLuint CompileAndLinkShaders(const PipelineDesc& desc);
    std::string ReadShaderSource(uint32_t shaderHandle); // placeholder for engine's shader storage
};


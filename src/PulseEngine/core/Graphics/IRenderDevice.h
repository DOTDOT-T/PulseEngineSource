#pragma once
#include "ICommandList.h"
#include "Handles.h"
#include "GpuDesc.h"

class IRenderDevice
{
public:
    virtual ~IRenderDevice() = default;

    // ---- Device lifecycle ----
    virtual bool Initialize(const char* appName, int width, int height) = 0;
    virtual void Shutdown() = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    // ---- Frame flow ----
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;

    // ---- Buffers ----
    virtual BufferHandle CreateBuffer(const GpuBufferDesc& desc, const void* initialData = nullptr) = 0;
    virtual void DestroyBuffer(BufferHandle h) = 0;

    // ---- Textures ----
    virtual TextureHandle CreateTexture(const GpuTextureDesc& desc) = 0;
    virtual void DestroyTexture(TextureHandle h) = 0;

    // ---- Pipelines ----
    virtual PipelineHandle CreatePipeline(const PipelineDesc& desc) = 0;
    virtual void DestroyPipeline(PipelineHandle h) = 0;

    // ---- Descriptor sets ----
    virtual DescriptorLayoutHandle CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) = 0;
    virtual DescriptorSetHandle AllocateDescriptorSet(DescriptorLayoutHandle layout) = 0;
    virtual void UpdateDescriptorSet(DescriptorSetHandle set, const DescriptorSetDesc& desc) = 0;

    // ---- Command lists ----
    virtual CommandListHandle CreateCommandList() = 0;
    virtual ICommandList* GetCommandList(CommandListHandle) = 0;
    virtual CommandQueueHandle GetGraphicsQueue() = 0;

    virtual void Submit(CommandQueueHandle queue, CommandListHandle list, FenceHandle fenceToSignal = 0) = 0;

    // ---- Sync ----
    virtual void WaitForFence(FenceHandle fence, uint64_t value) = 0;

    // ---- Utility ----
    virtual float GetTimeSeconds() const = 0;
};

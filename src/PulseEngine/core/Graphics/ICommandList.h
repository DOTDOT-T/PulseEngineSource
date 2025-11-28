#pragma once
#include "Handles.h"
#include "GpuDesc.h"

class ICommandList
{
public:
    virtual ~ICommandList() = default;

    virtual void Begin() = 0;
    virtual void End() = 0;

    // ---- Resource barriers (DX12/VK-friendly) ----
    struct ResourceBarrierDesc {
        TextureHandle texture = 0;
        BufferHandle buffer = 0;
        enum class State { Undefined, RenderTarget, DepthStencil, ShaderRead, TransferSrc, TransferDst };
        State before = State::Undefined;
        State after = State::Undefined;
    };
    virtual void ResourceBarrier(const ResourceBarrierDesc& desc) = 0;

    // ---- Pipeline ----
    virtual void BindPipeline(PipelineHandle pso) = 0;

    // ---- Descriptor sets ----
    virtual void BindDescriptorSet(uint32_t slot, DescriptorSetHandle set) = 0;

    // ---- Buffers ----
    virtual void BindVertexBuffer(BufferHandle buffer, uint64_t offset) = 0;
    virtual void BindIndexBuffer(BufferHandle buffer, uint64_t offset) = 0;

    // ---- Drawing ----
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                             uint32_t firstIndex, int32_t vertexOffset,
                             uint32_t firstInstance) = 0;

    // ---- Dispatch (compute) ----
    virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;

    // ---- Copy ----
    virtual void CopyBuffer(BufferHandle dst, uint64_t dstOffset,
                            BufferHandle src, uint64_t srcOffset,
                            size_t size) = 0;
};

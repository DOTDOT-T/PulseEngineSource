
#pragma once

#include "IRenderDevice.h"
#include "ICommandList.h"
#include <vector>

class GLCommandList : public ICommandList
{
public:
    GLCommandList();
    virtual ~GLCommandList();

    void Begin() override;
    void End() override;
    void ResourceBarrier(const ResourceBarrierDesc& desc) override;
    void BindPipeline(PipelineHandle pso) override;
    void BindDescriptorSet(uint32_t slot, DescriptorSetHandle set) override;
    void BindVertexBuffer(BufferHandle buffer, uint64_t offset) override;
    void BindIndexBuffer(BufferHandle buffer, uint64_t offset) override;
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance) override;
    void Dispatch(uint32_t x, uint32_t y, uint32_t z) override;
    void CopyBuffer(BufferHandle dst, uint64_t dstOffset,
                    BufferHandle src, uint64_t srcOffset,
                    size_t size) override;

    // Internal record structure
    struct Cmd { enum Type { BindPipeline, BindVB, BindIB, DrawIndexed, Dispatch, Barrier, CopyBuffer } type; 
                std::vector<uint8_t> data; };

    GLuint GetVAO() const { return vao; }
    // used by backend to replay recorded commands
    const std::vector<Cmd>& GetRecorded() const { return m_cmds; }
    void Clear();

private:
    std::vector<Cmd> m_cmds;
    GLuint vao = 0;
};
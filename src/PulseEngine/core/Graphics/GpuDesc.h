/**
 * @file GpuDesc.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include <vector>
#include "Handles.h"

struct GpuBufferDesc
{
    enum class Usage
    {
        Vertex,
        Index,
        Uniform,
        Storage,
        TransferSrc,
        TransferDst
    };

    Usage usage = Usage::Vertex;
    size_t size = 0;
    bool cpuWritable = false;
    bool gpuOnly = true;
};

struct GpuTextureDesc
{
    enum class Format
    {
        RGBA8,
        RGBA16F,
        Depth32F,
        BC1,
        BC3,
        UNKNOWN
    };

    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t mipLevels = 1;
    bool sampled = true;
    bool renderTarget = false;
    bool depthStencil = false;
    Format format = Format::RGBA8;
};
struct RasterState
{
    bool wireframe = false;
    bool cullBack = true;
};

struct DepthStencilState
{
    bool depthTest = true;
    bool depthWrite = true;
};

struct BlendState
{
    bool enable = false;
};
struct VertexAttribute
{
    enum class Type
    {
        Float,
        Float2,
        Float3,
        Float4,
        Mat4
    };
    std::string semantic;
    Type type;
    uint32_t offset;
};

struct VertexInputLayout
{
    uint32_t stride = 0;
    std::vector<VertexAttribute> attributes;
};
struct DescriptorEntry
{
    enum class Type
    {
        Sampler,
        Texture,
        UniformBuffer,
        StorageBuffer
    };
    Type type;
    uint32_t binding = 0;
};

struct DescriptorSetLayoutDesc
{
    std::vector<DescriptorEntry> entries;
};

struct DescriptorSetDesc
{
    struct Binding
    {
        uint32_t binding = 0;
        TextureHandle texture = 0;
        BufferHandle buffer = 0;
        size_t offset = 0;
        size_t range = 0;
    };
    std::vector<Binding> bindings;
};
struct PipelineDesc
{
    uint32_t shaderVS = 0;
    uint32_t shaderPS = 0;
    uint32_t shaderGS = 0;

    VertexInputLayout vertexLayout;
    RasterState raster;
    DepthStencilState depth;
    BlendState blend;

    DescriptorLayoutHandle descriptorLayout = 0;

    GpuTextureDesc::Format colorFormat = GpuTextureDesc::Format::RGBA8;
    GpuTextureDesc::Format depthFormat = GpuTextureDesc::Format::Depth32F;
};

struct VertexArrayDesc
{
    VertexInputLayout layout;
    BufferHandle vertexBuffer;
    BufferHandle indexBuffer;
};

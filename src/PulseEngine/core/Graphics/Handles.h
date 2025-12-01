#pragma once
#include <cstdint>
#include <string>
#include <vector>

using BufferHandle        = uint32_t;
using TextureHandle       = uint32_t;
using PipelineHandle      = uint32_t;
using DescriptorSetHandle = uint32_t;
using DescriptorLayoutHandle = uint32_t;
using CommandListHandle   = uint32_t;
using CommandQueueHandle  = uint32_t;
using VertexArrayHandle = uint32_t;
using FenceHandle         = uint64_t; // GPU timeline fence
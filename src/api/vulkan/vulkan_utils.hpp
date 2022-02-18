//
// Created by artyomd on 10/8/20.
//
#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "src/api/data_type.hpp"
#include "src/api/redering_pipeline_config.hpp"
#include "src/api/sampler.hpp"
#include "src/api/shader.hpp"
#include "src/utils/check.hpp"

#define VK_CALL(x)                                                  \
  {                                                                 \
    auto result = x;                                                \
    CHECK(result == VK_SUCCESS, "{} failed, result:{}", #x, result) \
  };
namespace api::vulkan {

VkBufferUsageFlags GetVkBufferUsage(api::BufferUsage buffer_usage);

VkMemoryPropertyFlags GetVkMemoryType(api::MemoryType memory_property);

VkIndexType GetVkType(api::DataType type);

VkFormat GetVkFormat(api::DataType type, uint32_t count);

VkFilter GetVkFilter(api::Filter filter);

VkPrimitiveTopology GetVkDrawMode(api::DrawMode draw_mode);

VkCullModeFlags GetVkCullMode(api::CullMode cull_mode);

VkFrontFace GetVkFrontFace(api::FrontFace front_face);

VkCompareOp GetVkCompareOp(api::CompareOp compare_op);

VkSamplerAddressMode GetVkAddressMode(api::AddressMode address_mode);

VkShaderStageFlagBits GetVkShaderStageFlag(api::ShaderType shader_type);

VkFormat GetVkFormat(api::PixelFormat pixel_format);

void CheckVkResult(VkResult result);
}  // namespace api::vulkan

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

namespace api::vulkan {

VkIndexType GetVkType(api::DataType type);

VkFormat GetVkFormat(api::DataType type, unsigned int count);

VkFilter GetVkFilter(api::Filter filter);

VkPrimitiveTopology GetVkDrawMode(api::DrawMode draw_mode);

VkCullModeFlags GetVkCullMode(api::CullMode cull_mode);

VkFrontFace GetVkFrontFace(api::FrontFace front_face);

VkCompareOp GetVkCompareOp(api::CompareOp compare_op);

VkSamplerAddressMode GetVkAddressMode(api::AddressMode address_mode);

VkShaderStageFlags GetVkShaderStageFlag(api::ShaderType shader_type);

VkShaderStageFlagBits GetShaderVkType(api::ShaderType shader_type);
}
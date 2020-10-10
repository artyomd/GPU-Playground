//
// Created by artyomd on 10/8/20.
//
#pragma once

#include <vulkan/vulkan.h>

#include "src/api/data_type.hpp"
#include "src/api/shader.hpp"

namespace api::vulkan {
VkIndexType GetVkType(api::DataType type);

VkFormat GetVkFormat(api::DataType type, unsigned int count);

VkShaderStageFlags GetVkShaderStageFlag(api::ShaderType shader_type);

VkShaderStageFlagBits GetShaderVkType(api::ShaderType shader_type);
}

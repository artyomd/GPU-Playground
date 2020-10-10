//
// Created by artyomd on 10/8/20.
//

#ifndef PLAYGROUND_SRC_API_VULKAN_VULKANUTILS_H_
#define PLAYGROUND_SRC_API_VULKAN_VULKANUTILS_H_

#include <vulkan/vulkan.h>

#include "src/api/DataType.h"
#include "src/api/Shader.h"

VkIndexType GetVkType(api::DataType type);

VkFormat GetVkFormat(api::DataType type, unsigned int count);

VkShaderStageFlags GetVkShaderStageFlag(api::ShaderType shader_type);

VkShaderStageFlagBits GetShaderVkType(api::ShaderType shader_type);

#endif //PLAYGROUND_SRC_API_VULKAN_VULKANUTILS_H_

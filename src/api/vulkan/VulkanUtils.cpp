//
// Created by artyomd on 10/8/20.
//

#include <stdexcept>

#include "src/api/vulkan/VulkanUtils.h"

VkFormat GetVkFormat(api::DataType type, unsigned int count) {
  switch (type) {
    case api::DataType::DATA_TYPE_BYTE:
      switch (count) {
        case 1:return VK_FORMAT_R8_UNORM;
        case 2:return VK_FORMAT_R8G8_UNORM;
        case 3:return VK_FORMAT_R8G8B8_UNORM;
        case 4:return VK_FORMAT_R8G8B8A8_UNORM;
        default:throw std::runtime_error("unsupported count");
      }
    case api::DataType::DATA_TYPE_UINT_16:
      switch (count) {
        case 1:return VK_FORMAT_R16_UINT;
        case 2:return VK_FORMAT_R16G16_UINT;
        case 3: return VK_FORMAT_R16G16B16_UINT;
        case 4: return VK_FORMAT_R16G16B16A16_UINT;
        default:throw std::runtime_error("unsupported count");
      }
    case api::DataType::DATA_TYPE_UINT_32:
      switch (count) {
        case 1:return VK_FORMAT_R32_UINT;
        case 2:return VK_FORMAT_R32G32_UINT;
        case 3: return VK_FORMAT_R32G32B32_UINT;
        case 4: return VK_FORMAT_R32G32B32A32_UINT;
        default:throw std::runtime_error("unsupported count");
      }
    case api::DataType::DATA_TYPE_FLOAT:
      switch (count) {
        case 1:return VK_FORMAT_R32_SFLOAT;
        case 2:return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:throw std::runtime_error("unsupported count");
      }
    default:throw std::runtime_error("unsupported enum");
  }
}

VkIndexType GetVkType(api::DataType type) {
  switch (type) {
    case api::DataType::DATA_TYPE_UINT_16:return VK_INDEX_TYPE_UINT16;
    case api::DataType::DATA_TYPE_UINT_32:return VK_INDEX_TYPE_UINT32;
    default:throw std::runtime_error("unsupported enum");
  }
}

VkShaderStageFlags GetVkShaderStageFlag(api::ShaderType shader_type) {
  switch (shader_type) {
    case api::ShaderType::SHADER_TYPE_VERTEX:return VK_SHADER_STAGE_VERTEX_BIT;
    case api::ShaderType::SHADER_TYPE_FRAGMENT:return VK_SHADER_STAGE_FRAGMENT_BIT;
    default: throw std::runtime_error("invalid shader type");
  }
}

VkShaderStageFlagBits GetShaderVkType(api::ShaderType shader_type) {
  switch (shader_type) {
    case api::ShaderType::SHADER_TYPE_VERTEX:return VK_SHADER_STAGE_VERTEX_BIT;
    case api::ShaderType::SHADER_TYPE_FRAGMENT:return VK_SHADER_STAGE_FRAGMENT_BIT;
    default: throw std::runtime_error("invalid shader type");
  }

}

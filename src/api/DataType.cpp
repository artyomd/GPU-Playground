//
// Created by artyomd on 1/6/20.
//

#include "DataType.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

int api::GetGlType(DataType type) {
  switch (type) {
    case DATA_TYPE_BYTE:return GL_UNSIGNED_BYTE;
    case DATA_TYPE_UINT_16:return GL_UNSIGNED_SHORT;
    case DATA_TYPE_UINT_32:return GL_UNSIGNED_INT;
    case DATA_TYPE_FLOAT:return GL_FLOAT;
  }
  throw std::runtime_error("unsupported enum");
}

VkFormat api::GetVkFormat(DataType type, int count) {
  switch (type) {
    case DATA_TYPE_BYTE:
      switch (count) {
        case 1:return VK_FORMAT_R8_UNORM;
        case 2:return VK_FORMAT_R8G8_UNORM;
        case 3:return VK_FORMAT_R8G8B8_UNORM;
        case 4:return VK_FORMAT_R8G8B8A8_UNORM;
        default:throw std::runtime_error("unsupported count");
      }
    case DATA_TYPE_UINT_16:
      switch (count) {
        case 1:return VK_FORMAT_R16_UINT;
        case 2:return VK_FORMAT_R16G16_UINT;
        case 3: return VK_FORMAT_R16G16B16_UINT;
        case 4: return VK_FORMAT_R16G16B16A16_UINT;
        default:throw std::runtime_error("unsupported count");
      }
    case DATA_TYPE_UINT_32:
      switch (count) {
        case 1:return VK_FORMAT_R32_UINT;
        case 2:return VK_FORMAT_R32G32_UINT;
        case 3: return VK_FORMAT_R32G32B32_UINT;
        case 4: return VK_FORMAT_R32G32B32A32_UINT;
        default:throw std::runtime_error("unsupported count");
      }
    case DATA_TYPE_FLOAT:
      switch (count) {
        case 1:return VK_FORMAT_R32_SFLOAT;
        case 2:return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:throw std::runtime_error("unsupported count");
      }
  }
  throw std::runtime_error("unsupported enum");
}

VkIndexType api::GetVkType(DataType type) {
  switch (type) {
    case DATA_TYPE_UINT_16:return VK_INDEX_TYPE_UINT16;
    case DATA_TYPE_UINT_32:return VK_INDEX_TYPE_UINT32;
  }
  throw std::runtime_error("unsupported enum");
}

int api::GetDataTypeSizeInBytes(DataType type) {
  switch (type) {
    case DATA_TYPE_BYTE:return 1;
    case DATA_TYPE_UINT_16:return 2;
    case DATA_TYPE_UINT_32:
    case DATA_TYPE_FLOAT:return 4;
  }
}
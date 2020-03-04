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

VkFormat api::GetVkType(DataType type) {
  switch (type) {
    case DATA_TYPE_BYTE:return VK_FORMAT_R8_UNORM;
    case DATA_TYPE_UINT_16:return VK_FORMAT_R16_UINT;
    case DATA_TYPE_UINT_32:return VK_FORMAT_R32_UINT;
    case DATA_TYPE_FLOAT:return VK_FORMAT_R32_SFLOAT;
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
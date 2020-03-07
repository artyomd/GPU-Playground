#pragma once

#include <vulkan/vulkan.h>

namespace api {
enum DataType {
  DATA_TYPE_BYTE,
  DATA_TYPE_UINT_16,//SHORT
  DATA_TYPE_UINT_32,
  DATA_TYPE_FLOAT
};

int GetGlType(DataType type);

VkIndexType GetVkType(DataType type);

VkFormat GetVkFormat(DataType type, int count);

int GetDataTypeSizeInBytes(DataType type);
}
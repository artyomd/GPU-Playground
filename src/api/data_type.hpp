#pragma once

#include <cstddef>

namespace api {
enum class DataType {
  DATA_TYPE_BYTE,
  DATA_TYPE_UINT_16,//SHORT
  DATA_TYPE_UINT_32,
  DATA_TYPE_FLOAT,
  COUNT,
};

enum class PixelFormat {
  PIXEL_FORMAT_R8G8B8A8_UNORM,
  PIXEL_FORMAT_R8G8B8A8_SRGB,
  COUNT,
};

struct Size {
  int width = 0;
  int height = 0;
};

size_t GetDataTypeSizeInBytes(DataType type);
size_t GetPixelFormatSizeInBytes(PixelFormat type);
}

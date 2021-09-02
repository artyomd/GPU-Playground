#pragma once

#include <cstddef>
#include <cstdint>

namespace api {
enum class DataType {
  BYTE,
  UINT_16,//SHORT
  UINT_32,
  FLOAT,
  COUNT,
};

enum class PixelFormat {
  RGBA_8_UNORM,
  RGBA_8_SRGB,
  COUNT,
};

struct Size {
  uint32_t width = 0;
  uint32_t height = 0;
};

size_t GetDataTypeSizeInBytes(DataType type);
size_t GetPixelFormatSizeInBytes(PixelFormat type);
}

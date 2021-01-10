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

struct Pixel_RGBA {
  float r, g, b, a;
};

struct Size {
  int width = 0;
  int height = 0;
};

size_t GetDataTypeSizeInBytes(DataType type);
}

//
// Created by artyomd on 1/6/20.
//

#include "src/api/data_type.hpp"

#include <stdexcept>

size_t api::GetDataTypeSizeInBytes(DataType type) {
  switch (type) {
    case DataType::BYTE:return 1;
    case DataType::UINT_16:return 2;
    case DataType::UINT_32:
    case DataType::FLOAT:return 4;
    default: throw std::runtime_error("unsupported enum");
  }
}

size_t api::GetPixelFormatSizeInBytes(PixelFormat type) {
  switch (type) {
    case PixelFormat::RGBA_8_UNORM:
    case PixelFormat::RGBA_8_SRGB:return 4;
    default:throw std::runtime_error("unsupported enum");
  }
}

//
// Created by artyomd on 1/6/20.
//

#include "src/api/data_type.hpp"

#include <stdexcept>

size_t api::GetDataTypeSizeInBytes(DataType type) {
  switch (type) {
    case DataType::DATA_TYPE_BYTE:return 1;
    case DataType::DATA_TYPE_UINT_16:return 2;
    case DataType::DATA_TYPE_UINT_32:
    case DataType::DATA_TYPE_FLOAT:return 4;
    default: throw std::runtime_error("unsupported enum");
  }
}

size_t api::GetPixelFormatSizeInBytes(PixelFormat type) {
  switch (type) {
    case PixelFormat::PIXEL_FORMAT_R8G8B8A8_UNORM:
    case PixelFormat::PIXEL_FORMAT_R8G8B8A8_SRGB:return 4;
    default:throw std::runtime_error("unsupported enum");
  }
}

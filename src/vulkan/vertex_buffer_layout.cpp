#include "vertex_buffer_layout.hpp"

#include <stdexcept>

size_t vulkan::GetDataTypeSizeInBytes(const DataType& type) {
  switch (type) {
    case DataType::BYTE:
    case DataType::UINT_8:
      return 1;
    case DataType::UINT_16:
      return 2;
    case DataType::UINT_32:
    case DataType::FLOAT:
      return 4;
    default:
      throw std::runtime_error("unsupported enum");
  }
}

const std::vector<vulkan::VertexAttribute>& vulkan::VertexBufferLayout::GetElements() const { return elements_; }

void vulkan::VertexBufferLayout::Push(const VertexAttribute& attribute) { elements_.push_back(attribute); }

size_t vulkan::VertexBufferLayout::GetElementSize() const {
  size_t size = 0;
  for (const auto& elem : elements_) {
    size += elem.count * GetDataTypeSizeInBytes(elem.type);
  }
  return size;
}

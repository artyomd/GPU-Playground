#pragma once

#include <stdexcept>
#include <vector>

namespace vulkan {
enum class DataType {
  BYTE, UINT_16,
  UINT_32, FLOAT,
};

size_t GetDataTypeSizeInBytes(DataType type);

struct VertexAttribute {
  unsigned int binding_index;
  DataType type;
  size_t count;
};

class VertexBufferLayout {
 public:
  VertexBufferLayout() = default;

  void Push(VertexAttribute attribute);

  [[nodiscard]] size_t GetElementSize() const;

  [[nodiscard]] const std::vector<VertexAttribute> &GetElements() const;
 private:
  std::vector<VertexAttribute> elements_{};
};
}  // namespace vulkan
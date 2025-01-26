#pragma once

#include <cstdint>
#include <vector>

namespace vulkan {
enum class DataType : std::uint8_t {
  BYTE,
  UINT_8,
  UINT_16,
  UINT_32,
  FLOAT,
};

size_t GetDataTypeSizeInBytes(const DataType& type);

struct VertexAttribute {
  unsigned int binding_index;
  DataType type;
  size_t count;
};

class VertexBufferLayout final {
 public:
  VertexBufferLayout() = default;

  void Push(const VertexAttribute& attribute);

  [[nodiscard]] size_t GetElementSize() const;

  [[nodiscard]] const std::vector<VertexAttribute>& GetElements() const;

 private:
  std::vector<VertexAttribute> elements_;
};
}  // namespace vulkan

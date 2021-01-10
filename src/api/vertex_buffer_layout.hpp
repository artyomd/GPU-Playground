//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include <stdexcept>
#include <vector>

#include "src/api/data_type.hpp"

namespace api {
struct VertexAttribute {
  DataType type;
  size_t count;
  size_t offset;
  size_t stride;
};

class VertexBufferLayout {
 private:
  std::vector<VertexAttribute> elements_{};
 public:
  VertexBufferLayout() = default;

  void Push(const VertexAttribute &vertex_attribute);

  [[nodiscard]] const std::vector<VertexAttribute> &GetElements() const;
};
}

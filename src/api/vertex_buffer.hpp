//
// Created by Artyom Dangizyan on 10/8/18.
//
#pragma once

#include <utility>

#include "src/api/vertex_buffer_layout.hpp"

namespace api {
class VertexBuffer {
 protected:
  size_t size_in_bytes_;

  explicit VertexBuffer(size_t size_in_bytes)
      : size_in_bytes_(size_in_bytes) {}

 public:
  virtual void Update(const void *data) = 0;

  virtual ~VertexBuffer() = default;
};
}

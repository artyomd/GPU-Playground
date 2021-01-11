//
// Created by Artyom Dangizyan on 10/8/18.
//
#pragma once

#include <utility>

#include "src/api/buffer.hpp"

namespace api {
class VertexBuffer : public virtual Buffer {
 public:
  VertexBuffer() = delete;
 protected:
 public:
  explicit VertexBuffer(const size_t &size_in_bytes) : Buffer(size_in_bytes) {}
};
}

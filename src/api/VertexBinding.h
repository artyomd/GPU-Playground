#pragma once

#include "src/api/VertexBuffer.h"
#include "src/api/VertexBufferLayout.cpp"

namespace api {
class VertexBinding {
 protected:
  const VertexBuffer *buffer_;
  const VertexBufferLayout *buffer_description_;
 public:
  VertexBinding(const VertexBuffer *buffer, const VertexBufferLayout *description) :
      buffer_(buffer), buffer_description_(description) {};

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual ~VertexBinding() = default;
};
}
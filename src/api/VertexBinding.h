#pragma once

#include <utility>
#include <memory>

#include "src/api/VertexBuffer.h"
#include "src/api/VertexBufferLayout.cpp"

namespace api {
class VertexBinding {
 protected:
  std::shared_ptr<VertexBuffer> buffer_;
  std::shared_ptr<VertexBufferLayout> buffer_description_;
 public:
  VertexBinding(std::shared_ptr<VertexBuffer> buffer,
                std::shared_ptr<VertexBufferLayout> buffer_description)
      : buffer_(std::move(buffer)), buffer_description_(std::move(buffer_description)) {}

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual ~VertexBinding() = default;
};
}
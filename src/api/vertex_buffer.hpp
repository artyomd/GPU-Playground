//
// Created by Artyom Dangizyan on 10/8/18.
//
#pragma once

namespace api {
class VertexBuffer {
 public:
  VertexBuffer() = default;

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual ~VertexBuffer() = default;
};
}
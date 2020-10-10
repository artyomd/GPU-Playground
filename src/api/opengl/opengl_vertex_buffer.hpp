//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <GL/glew.h>

#include "src/api/vertex_buffer.hpp"

namespace api::opengl {
class OpenGlVertexBuffer : public VertexBuffer {
 private:
  GLuint renderer_id_ = -1;
 public:
  OpenGlVertexBuffer(const void *data, unsigned int size);

  void Bind() const override;

  void Unbind() const override;

  ~OpenGlVertexBuffer() override;
};
}
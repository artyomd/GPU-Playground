//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <GL/glew.h>

#include "src/api/vertex_buffer.hpp"

namespace api::opengl {
class OpenGlVertexBuffer : public VertexBuffer {
 private:
  GLuint buffer_id_ = 0;
  GLuint vertex_array_id_ = 0;
 public:
  OpenGlVertexBuffer(size_t size_in_bytes, const VertexBufferLayout& layout);

  void Update(const void *data) override;
  [[nodiscard]] GLuint GetBufferId() const;
  [[nodiscard]] GLuint GetVertexArrayId() const;

  ~OpenGlVertexBuffer() override;
};
}

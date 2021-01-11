//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <GL/glew.h>

#include "src/api/vertex_buffer.hpp"
#include "src/api/vertex_buffer_layout.hpp"
#include "src/api/opengl/opengl_buffer.hpp"

namespace api::opengl {
class OpenGlVertexBuffer :
    public OpenGlBuffer,
    public VertexBuffer {
 private:
  GLuint vertex_array_id_ = 0;
 public:
  OpenGlVertexBuffer(size_t size_in_bytes, const VertexBufferLayout &layout);

  [[nodiscard]] GLuint GetVertexArrayId() const;

  ~OpenGlVertexBuffer() override;
};
}

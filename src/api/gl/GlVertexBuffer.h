//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <api/VertexBuffer.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace api {
class GlVertexBuffer : public VertexBuffer {
 private:
  GLuint renderer_id_ = -1;
 public:
  GlVertexBuffer(const void *data, unsigned int size);

  void Bind() const override;

  void Unbind() const override;

  ~GlVertexBuffer() override;
};
}
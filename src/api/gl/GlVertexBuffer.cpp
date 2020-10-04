//
// Created by artyomd on 12/30/19.
//

#include "src/api/gl/GlVertexBuffer.h"

#include "src/api/gl/GlUtils.h"

namespace api {
GlVertexBuffer::GlVertexBuffer(const void *data, unsigned int size) {
  GL_CALL(glGenBuffers(1, &renderer_id_));
  assert(renderer_id_ != 0);
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void GlVertexBuffer::Bind() const {
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
}

void GlVertexBuffer::Unbind() const {
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

GlVertexBuffer::~GlVertexBuffer() {
  GL_CALL(glDeleteBuffers(1, &renderer_id_));
}
}
//
// Created by artyomd on 12/30/19.
//

#include "src/api/opengl/opengl_vertex_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlVertexBuffer::OpenGlVertexBuffer(const void *data, unsigned int size) {
  GL_CALL(glGenBuffers(1, &renderer_id_));
  assert(renderer_id_ != 0);
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void api::opengl::OpenGlVertexBuffer::Bind() const {
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
}

void api::opengl::OpenGlVertexBuffer::Unbind() const {
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

api::opengl::OpenGlVertexBuffer::~OpenGlVertexBuffer() {
  GL_CALL(glDeleteBuffers(1, &renderer_id_));
}
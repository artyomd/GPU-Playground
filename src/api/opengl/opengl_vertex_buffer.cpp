//
// Created by artyomd on 12/30/19.
//

#include "src/api/opengl/opengl_vertex_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlVertexBuffer::OpenGlVertexBuffer(size_t size_in_bytes, const VertexBufferLayout &layout)
    : Buffer(size_in_bytes),
      OpenGlBuffer(size_in_bytes),
      VertexBuffer(size_in_bytes) {
  GL_CALL(glGenVertexArrays(1, &vertex_array_id_));
  GL_CALL(glBindVertexArray(vertex_array_id_));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer_id_));
  const auto &elements = layout.GetElements();
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];
    GL_CALL(glEnableVertexAttribArray(i));
    GL_CALL(glVertexAttribPointer(i, element.count, GetGlType(element.type), GL_FALSE,
                                  element.stride,
                                  reinterpret_cast<void *> (element.offset)));
  }
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CALL(glBindVertexArray(0));
}

GLuint api::opengl::OpenGlVertexBuffer::GetVertexArrayId() const {
  return vertex_array_id_;
}

api::opengl::OpenGlVertexBuffer::~OpenGlVertexBuffer() {
  GL_CALL(glDeleteVertexArrays(1, &vertex_array_id_));
}

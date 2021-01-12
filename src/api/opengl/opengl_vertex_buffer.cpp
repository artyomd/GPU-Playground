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
  auto stride = static_cast<GLsizei>(layout.GetElementSize());
  size_t offset = 0;
  for (auto element : elements) {
    GL_CALL(glEnableVertexAttribArray(element.binding_index));
    GL_CALL(glVertexAttribPointer(element.binding_index,
                                  element.count,
                                  GetGlType(element.type), GL_FALSE,
                                  stride,
                                  reinterpret_cast<void *> (offset)));
    offset += element.count * api::GetDataTypeSizeInBytes(element.type);
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

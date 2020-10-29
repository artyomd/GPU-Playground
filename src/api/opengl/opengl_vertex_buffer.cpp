//
// Created by artyomd on 12/30/19.
//

#include "src/api/opengl/opengl_vertex_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlVertexBuffer::OpenGlVertexBuffer(size_t size_in_bytes, const VertexBufferLayout &layout) :
    VertexBuffer(size_in_bytes) {
  GL_CALL(glGenBuffers(1, &buffer_id_));
  assert(buffer_id_ != 0);

  GL_CALL(glGenVertexArrays(1, &vertex_array_id_));
  GL_CALL(glBindVertexArray(vertex_array_id_));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer_id_));
  const auto &elements = layout.GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];
    GL_CALL(glEnableVertexAttribArray(i));
    GL_CALL(glVertexAttribPointer(i, element.count, GetGlType(element.type), GL_FALSE, layout.GetStride(),
                                  reinterpret_cast<void *> (offset)));
    offset += element.count * GetDataTypeSizeInBytes(element.type);
  }
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CALL(glBindVertexArray(0));
}

void api::opengl::OpenGlVertexBuffer::Update(void *data) {
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer_id_));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, size_in_bytes_, data, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

GLuint api::opengl::OpenGlVertexBuffer::GetBufferId() const {
  return buffer_id_;
}
GLuint api::opengl::OpenGlVertexBuffer::GetVertexArrayId() const {
  return vertex_array_id_;
}

api::opengl::OpenGlVertexBuffer::~OpenGlVertexBuffer() {
  GL_CALL(glDeleteBuffers(1, &buffer_id_));
  GL_CALL(glDeleteVertexArrays(1, &vertex_array_id_));
}

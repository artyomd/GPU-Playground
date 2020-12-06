//
// Created by Artyom Dangizyan on 10/8/18.
//

#include "src/api/opengl/opengl_index_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlIndexBuffer::OpenGlIndexBuffer(unsigned int item_count, enum DataType type)
    : IndexBuffer(item_count, type) {
  GL_CALL(glGenBuffers(1, &buffer_id_));
  assert(buffer_id_ != 0);
  this->index_type_ = GetGlType(type);
}

void api::opengl::OpenGlIndexBuffer::Update(const void *data) {
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id_));
  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes_, data, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

GLuint api::opengl::OpenGlIndexBuffer::GetBufferId() const {
  return buffer_id_;
}
GLint api::opengl::OpenGlIndexBuffer::GetIndexType() const {
  return index_type_;
}

api::opengl::OpenGlIndexBuffer::~OpenGlIndexBuffer() {
  GL_CALL(glDeleteBuffers(1, &buffer_id_));
}

//
// Created by Artyom Dangizyan on 10/8/18.
//

#include "src/api/opengl/opengl_index_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlIndexBuffer::OpenGlIndexBuffer(const void *data, unsigned int item_count, enum DataType type)
    : IndexBuffer(item_count, type) {
  GL_CALL(glGenBuffers(1, &renderer_id_));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetDataTypeSizeInBytes(type) * item_count, data, GL_STATIC_DRAW));
}

void api::opengl::OpenGlIndexBuffer::Bind() const {
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
}

void api::opengl::OpenGlIndexBuffer::Unbind() const {
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

api::opengl::OpenGlIndexBuffer::~OpenGlIndexBuffer() {
  GL_CALL(glDeleteBuffers(1, &renderer_id_));
}

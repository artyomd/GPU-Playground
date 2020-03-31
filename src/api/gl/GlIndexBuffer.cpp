//
// Created by Artyom Dangizyan on 10/8/18.
//

#include <GL/glew.h>
#include "GlIndexBuffer.h"
#include "GlUtils.h"

namespace api {
GlIndexBuffer::GlIndexBuffer(const void *data, unsigned int item_count, enum DataType type)
    : IndexBuffer(item_count, type) {
  GL_CALL(glGenBuffers(1, &renderer_id_));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
  GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetDataTypeSizeInBytes(type)*item_count, data, GL_STATIC_DRAW));
}

GlIndexBuffer::~GlIndexBuffer() {
  GL_CALL(glDeleteBuffers(1, &renderer_id_));
}

void GlIndexBuffer::Bind() const {
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
}

void GlIndexBuffer::Unbind() const {
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
}
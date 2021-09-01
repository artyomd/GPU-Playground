//
// Created by Artyom Dangizyan on 10/8/18.
//

#include "src/api/opengl/opengl_index_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlIndexBuffer::OpenGlIndexBuffer(uint32_t item_count, enum DataType type)
    : Buffer(item_count * GetDataTypeSizeInBytes(type)),
      OpenGlBuffer(item_count * GetDataTypeSizeInBytes(type)),
      IndexBuffer(item_count, type) {
  this->index_type_ = GetGlType(type);
}
GLenum api::opengl::OpenGlIndexBuffer::GetIndexType() const {
  return index_type_;
}

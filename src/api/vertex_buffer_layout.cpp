//
// Created by Artyom Dangizyan on 10/22/18.
//

#include "src/api/vertex_buffer_layout.hpp"

template<>
void api::VertexBufferLayout::Push<float>(unsigned int count) {
  elements_.push_back({DataType::DATA_TYPE_FLOAT, count});
  stride_ += count * GetDataTypeSizeInBytes(DataType::DATA_TYPE_FLOAT);
}

template<>
void api::VertexBufferLayout::Push<unsigned int>(unsigned int count) {
  elements_.push_back({DataType::DATA_TYPE_UINT_32, count});
  stride_ += count * GetDataTypeSizeInBytes(DataType::DATA_TYPE_UINT_32);
}
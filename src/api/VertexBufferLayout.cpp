//
// Created by Artyom Dangizyan on 10/22/18.
//

#include "src/api/VertexBufferLayout.h"

template<>
inline void api::VertexBufferLayout::Push<float>(unsigned int count) {
  elements_.push_back({DATA_TYPE_FLOAT, count});
  stride_ += count * GetDataTypeSizeInBytes(DATA_TYPE_FLOAT);
}

template<>
inline void api::VertexBufferLayout::Push<unsigned int>(unsigned int count) {
  elements_.push_back({DATA_TYPE_UINT_32, count});
  stride_ += count * GetDataTypeSizeInBytes(DATA_TYPE_UINT_32);
}
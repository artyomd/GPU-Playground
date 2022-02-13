//
// Created by Artyom Dangizyan on 10/22/18.
//

#include "src/api/vertex_buffer_layout.hpp"

const std::vector<api::VertexAttribute> &api::VertexBufferLayout::GetElements()
    const {
  return elements_;
}

void api::VertexBufferLayout::Push(api::VertexAttribute attribute) {
  elements_.emplace_back(attribute);
}

size_t api::VertexBufferLayout::GetElementSize() const {
  size_t size = 0;
  for (auto elem : elements_) {
    size += elem.count * api::GetDataTypeSizeInBytes(elem.type);
  }
  return size;
}

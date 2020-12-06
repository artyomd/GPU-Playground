//
// Created by Artyom Dangizyan on 10/22/18.
//

#include "src/api/vertex_buffer_layout.hpp"

const std::vector<api::VertexAttribute> &api::VertexBufferLayout::GetElements() const {
  return elements_;
}
void api::VertexBufferLayout::Push(const api::VertexAttribute &vertex_attribute) {
  elements_.emplace_back(vertex_attribute);
}

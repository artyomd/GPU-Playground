//
// Created by artyomd on 8/15/21.
//

#include "src/geometry/geometry_item.hpp"

std::shared_ptr<api::VertexBuffer> geometry::GeometryItem::GetVertexBuffer() const {
  return this->vertex_buffer_;
}
std::shared_ptr<api::IndexBuffer> geometry::GeometryItem::GetIndexBuffer() const {
  return this->index_buffer_;
}
geometry::GeometryItem::~GeometryItem() = default;

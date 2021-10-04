//
// Created by artyomd on 8/15/21.
//

#include "src/geometry/geometry_item.hpp"

std::shared_ptr<api::Buffer> geometry::GeometryItem::GetVertexBuffer() const {
  return this->vertex_buffer_;
}
std::shared_ptr<api::Buffer> geometry::GeometryItem::GetIndexBuffer() const {
  return this->index_buffer_;
}
api::DataType geometry::GeometryItem::GetIndexBufferDataType() const {
  return index_buffer_data_type_;
}
const api::VertexBufferLayout &geometry::GeometryItem::GetVbl() const {
  return vbl_;
}

size_t geometry::GeometryItem::GetIndexCount() const {
  return index_count_;
}

geometry::GeometryItem::~GeometryItem() = default;

#include "geometry/geometry_item.hpp"

geometry::GeometryItem::GeometryItem(const std::shared_ptr<vulkan::RenderingContext> &context) : context_(context) {}\

std::shared_ptr<vulkan::Buffer> geometry::GeometryItem::GetVertexBuffer() const {
  return vertex_buffer_;
}
std::shared_ptr<vulkan::Buffer> geometry::GeometryItem::GetIndexBuffer() const {
  return index_buffer_;
}
VkIndexType geometry::GeometryItem::GetIndexBufferDataType() const {
  return index_buffer_data_type_;
}
vulkan::VertexBufferLayout geometry::GeometryItem::GetVbl() const {
  return vbl_;
}

size_t geometry::GeometryItem::GetIndexCount() const { return index_count_; }

geometry::GeometryItem::~GeometryItem() = default;

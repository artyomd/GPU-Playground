#pragma once

#include "vulkan/buffer.hpp"
#include "vulkan/rendering_context.hpp"
#include "vulkan/vertex_buffer_layout.hpp"

namespace geometry {
class GeometryItem {
 protected:
  std::shared_ptr<vulkan::RenderingContext> context_;

  std::shared_ptr<vulkan::Buffer> vertex_buffer_ = nullptr;
  vulkan::VertexBufferLayout vbl_;

  std::shared_ptr<vulkan::Buffer> index_buffer_ = nullptr;
  VkIndexType index_buffer_data_type_ = VK_INDEX_TYPE_UINT16;
  size_t index_count_ = 0;

 public:
  GeometryItem() = delete;
  GeometryItem(const GeometryItem&) = delete;
  GeometryItem(GeometryItem&&) = delete;
  GeometryItem& operator=(const GeometryItem&) = delete;
  GeometryItem& operator=(GeometryItem&&) = delete;

  explicit GeometryItem(const std::shared_ptr<vulkan::RenderingContext>& context);

  [[nodiscard]] std::shared_ptr<vulkan::Buffer> GetVertexBuffer() const;
  [[nodiscard]] const vulkan::VertexBufferLayout& GetVbl() const;
  [[nodiscard]] std::shared_ptr<vulkan::Buffer> GetIndexBuffer() const;
  [[nodiscard]] VkIndexType GetIndexBufferDataType() const;
  [[nodiscard]] size_t GetIndexCount() const;

  virtual ~GeometryItem() = 0;
};
}  // namespace geometry

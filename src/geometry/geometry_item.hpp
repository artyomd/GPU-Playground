//
// Created by artyomd on 12/31/18.
//

#pragma once

#include <utility>

#include "src/api/rendering_context.hpp"

namespace geometry {
class GeometryItem {
 protected:
  std::shared_ptr<api::Buffer> vertex_buffer_ = nullptr;
  api::VertexBufferLayout vbl_{};

  std::shared_ptr<api::Buffer> index_buffer_ = nullptr;
  api::DataType index_buffer_data_type_ = api::DataType::UINT_16;
  size_t index_count_ = 0;

  std::shared_ptr<api::RenderingContext> context_;

 public:
  explicit GeometryItem(std::shared_ptr<api::RenderingContext> context)
      : context_(std::move(context)) {}

  [[nodiscard]] std::shared_ptr<api::Buffer> GetVertexBuffer() const;

  [[nodiscard]] std::shared_ptr<api::Buffer> GetIndexBuffer() const;

  [[nodiscard]] api::DataType GetIndexBufferDataType() const;

  [[nodiscard]] const api::VertexBufferLayout &GetVbl() const;

  [[nodiscard]] size_t GetIndexCount() const;

  virtual ~GeometryItem() = 0;
};
}  // namespace geometry

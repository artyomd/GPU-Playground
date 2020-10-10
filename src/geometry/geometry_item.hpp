//
// Created by artyomd on 12/31/18.
//

#pragma once

#include <src/api/index_buffer.hpp>
#include <src/api/rendering_context.hpp>
#include <src/api/vertex_binding.hpp>

#include <utility>

namespace geometry {
class GeometryItem {
 protected:
  std::shared_ptr<api::VertexBinding> vertex_binding_ = nullptr;
  std::shared_ptr<api::IndexBuffer> index_buffer_ = nullptr;
  std::shared_ptr<api::RenderingContext> context_;
 public:
  explicit GeometryItem(std::shared_ptr<api::RenderingContext> context) : context_(std::move(context)) {}

  [[nodiscard]] inline std::shared_ptr<api::VertexBinding> GetVertexBinding() const {
    return vertex_binding_;
  }

  [[nodiscard]] inline std::shared_ptr<api::IndexBuffer> GetIndexBuffer() const {
    return index_buffer_;
  }
};
}
//
// Created by artyomd on 12/31/18.
//

#pragma once

#include <src/api/IndexBuffer.h>
#include <src/api/RenderingContext.h>
#include <src/api/VertexBinding.h>

namespace geometry {
class GeometryItem {
 protected:
  api::VertexBinding *vertex_binding_ = nullptr;
  api::IndexBuffer *index_buffer_ = nullptr;
  api::RenderingContext *context_;
 public:
  explicit GeometryItem(api::RenderingContext *context) : context_(context) {}

  [[nodiscard]] inline const api::VertexBinding *GetVertexBinding() const {
    return vertex_binding_;
  }

  [[nodiscard]] inline const api::IndexBuffer *GetIndexBuffer() const {
    return index_buffer_;
  }

  virtual ~GeometryItem() = default;
};
}
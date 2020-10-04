//
// Created by artyomd on 3/23/20.
//
#pragma once

#include "src/geometry/GeometryItem.h"

namespace geometry {
class StackedSphere : public GeometryItem {
 private:
  api::VertexBuffer *vertex_buffer_ = nullptr;
  api::VertexBufferLayout *layout_ = nullptr;
 public:
  explicit StackedSphere(api::RenderingContext *context,
                         float radius = 1.0f,
                         unsigned int stacks = 8,
                         unsigned int slices = 16);

  ~StackedSphere() override;
};
}
//
// Created by artyomd on 3/22/20.
//
#pragma once

#include "src/geometry/GeometryItem.h"

namespace geometry {
class SpiralSphere : public GeometryItem {
 private:
  api::VertexBuffer *vertex_buffer_ = nullptr;
  api::VertexBufferLayout *layout_ = nullptr;
 public:
  explicit SpiralSphere(api::RenderingContext *context,
                        float radius = 1.0f,
                        unsigned int loops = 8,
                        unsigned int segments_per_loop = 8);

  ~SpiralSphere() override;
};
}
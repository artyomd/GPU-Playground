//
// Created by artyomd on 3/22/20.
//
#pragma once

#include "src/geometry/geometry_item.hpp"

namespace geometry {
class SpiralSphere : public GeometryItem {
 private:
  std::shared_ptr<api::VertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<api::VertexBufferLayout> layout_ = nullptr;
 public:
  explicit SpiralSphere(std::shared_ptr<api::RenderingContext> context,
                        float radius = 1.0f,
                        unsigned int loops = 8,
                        unsigned int segments_per_loop = 8);
};
}
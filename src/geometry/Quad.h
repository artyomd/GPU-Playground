//
// Created by artyomd on 1/6/20.
//
#pragma once

#include "src/geometry/GeometryItem.h"
#include "src/geometry/Point.hpp"

namespace geometry {
class Quad : public GeometryItem {
 private:
  std::shared_ptr<api::VertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<api::VertexBufferLayout> layout_ = nullptr;
 public:
  explicit Quad(const std::shared_ptr<api::RenderingContext>& context,
                Point &top_left,
                Point &top_right,
                Point &bottom_right,
                Point &bottom_left);
};
}
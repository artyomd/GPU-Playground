//
// Created by artyomd on 1/6/20.
//
#pragma once

#include "src/geometry/GeometryItem.h"
#include "src/geometry/Point.hpp"

namespace geometry {
class Quad : public GeometryItem {
 private:
  api::VertexBuffer *vertex_buffer_ = nullptr;
  api::VertexBufferLayout *layout_ = nullptr;
 public:
  explicit Quad(api::RenderingContext *context,
                Point &top_left,
                Point &top_right,
                Point &bottom_right,
                Point &bottom_left);

  ~Quad() override;
};
}
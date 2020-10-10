//
// Created by artyomd on 1/6/20.
//

#pragma once

#include "src/api/rendering_context.hpp"
#include "src/geometry/geometry_item.hpp"
#include "src/geometry/point.hpp"

namespace geometry {
class Triangle : public GeometryItem {
 private:
  std::shared_ptr<api::VertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<api::VertexBufferLayout> layout_ = nullptr;
 public:
  explicit Triangle(std::shared_ptr<api::RenderingContext> context,
                    Point &point_0,
                    Point &point_1,
                    Point &point_2);
};
}

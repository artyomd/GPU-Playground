#pragma once

#include "src/geometry/geometry_item.hpp"
#include "src/geometry/point.hpp"

namespace geometry {
class Quad : public GeometryItem {
 public:
  explicit Quad(const std::shared_ptr<api::RenderingContext> &context,
                Point &top_left, Point &top_right, Point &bottom_right,
                Point &bottom_left);
};
}  // namespace geometry

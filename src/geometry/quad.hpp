#pragma once

#include "geometry/geometry_item.hpp"
#include "geometry/point.hpp"

namespace geometry {
class Quad : public GeometryItem {
 public:
  explicit Quad(const std::shared_ptr<vulkan::RenderingContext> &context,
                const Point &point_0,
                const Point &point_1,
                const Point &point_2,
                const Point &point_3);
};
}  // namespace geometry

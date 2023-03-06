#pragma once

#include "geometry/geometry_item.hpp"
#include "geometry/point.hpp"

namespace geometry {
class Triangle : public GeometryItem {
 public:
  explicit Triangle(const std::shared_ptr<vulkan::RenderingContext> &context,
                    const Point &point_0,
                    const Point &point_1,
                    const Point &point_2);
};
}  // namespace geometry

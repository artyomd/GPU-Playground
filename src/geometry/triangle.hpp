#pragma once

#include "geometry/geometry_item.hpp"
#include "geometry/point.hpp"

namespace geometry {
class Triangle final : public GeometryItem {
 public:
  explicit Triangle(const std::shared_ptr<vulkan::RenderingContext> &context, const PointWithColor &point_0,
                    const PointWithColor &point_1, const PointWithColor &point_2);
};
}  // namespace geometry

//
// Created by artyomd on 1/6/20.
//

#pragma once

#include "src/api/rendering_context.hpp"
#include "src/geometry/geometry_item.hpp"
#include "src/geometry/point.hpp"

namespace geometry {
class Triangle : public GeometryItem {
 public:
  explicit Triangle(const std::shared_ptr<api::RenderingContext> &context,
                    const Point &point_0, const Point &point_1,
                    const Point &point_2);
};
}  // namespace geometry

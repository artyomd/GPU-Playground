//
// Created by artyomd on 3/23/20.
//
#pragma once

#include "src/geometry/geometry_item.hpp"

namespace geometry {
class StackedSphere : public GeometryItem {
 public:
  explicit StackedSphere(const std::shared_ptr<api::RenderingContext> &context,
                         float radius = 1.0f, unsigned int stacks = 8,
                         unsigned int slices = 16);
};
}  // namespace geometry

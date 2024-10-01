#pragma once

#include "geometry_item.hpp"

namespace geometry {
class StackedSphere final : public GeometryItem {
 public:
  explicit StackedSphere(const std::shared_ptr<vulkan::RenderingContext> &context, float radius = 1.0,
                         unsigned int stacks = 8, unsigned int slices = 16);
};
}  // namespace geometry

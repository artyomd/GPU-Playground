#pragma once

#include "geometry_item.hpp"

namespace geometry {
class SpiralSphere final : public GeometryItem {
 public:
  explicit SpiralSphere(const std::shared_ptr<vulkan::RenderingContext> &context, float radius = 1.0,
                        unsigned int loops = 8, unsigned int segments_per_loop = 8);
};
}  // namespace geometry

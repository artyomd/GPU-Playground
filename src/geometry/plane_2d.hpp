#pragma once

#include "geometry_item.hpp"

namespace geometry {
class Plane2d final : public GeometryItem {
 public:
  explicit Plane2d(const std::shared_ptr<vulkan::RenderingContext>& context, const uint32_t& width,
                   const uint32_t& height);
};
}  // namespace geometry

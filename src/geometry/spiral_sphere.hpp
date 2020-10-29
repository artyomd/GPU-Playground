//
// Created by artyomd on 3/22/20.
//
#pragma once

#include "src/geometry/geometry_item.hpp"

namespace geometry {
class SpiralSphere : public GeometryItem {
 public:
  explicit SpiralSphere(std::shared_ptr<api::RenderingContext> context,
                        float radius = 1.0f,
                        unsigned int loops = 8,
                        unsigned int segments_per_loop = 8);
};
}

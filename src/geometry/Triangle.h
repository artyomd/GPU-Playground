//
// Created by artyomd on 1/6/20.
//

#pragma once

#include "src/api/RenderingContext.h"
#include "src/geometry/GeometryItem.h"
#include "src/geometry/Point.hpp"

namespace geometry {
class Triangle : public GeometryItem {
 private:
  api::VertexBuffer *vertex_buffer_ = nullptr;
  api::VertexBufferLayout *layout_ = nullptr;
 public:
  explicit Triangle(api::RenderingContext *context,
                    Point &point_0,
                    Point &point_1,
                    Point &point_2);

  ~Triangle() override;
};
}

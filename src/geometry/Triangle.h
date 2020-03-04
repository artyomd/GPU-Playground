//
// Created by artyomd on 1/6/20.
//

#pragma once

#include "GeometryItem.h"
#include "Point.hpp"
#include "RenderingContext.h"

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

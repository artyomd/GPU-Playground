//
// Created by artyomd on 1/6/20.
//

#include "src/geometry/quad.hpp"

geometry::Quad::Quad(const std::shared_ptr<api::RenderingContext> &context,
                     geometry::Point &top_left,
                     geometry::Point &top_right,
                     geometry::Point &bottom_right,
                     geometry::Point &bottom_left) : GeometryItem(context) {
  api::VertexBufferLayout layout;
  layout.Push<float>(3);
  layout.Push<float>(4);
  vertex_buffer_ = context->CreateVertexBuffer(4 * 7 * sizeof(float), layout);
  index_buffer_ = context->CreateIndexBuffer(6, api::DataType::DATA_TYPE_UINT_16);

  float positions[] = {
      top_left.x, top_left.y, top_left.z, top_left.r, top_left.g, top_left.b, top_left.a,
      top_right.x, top_right.y, top_right.z, top_right.r, top_right.g, top_right.b, top_right.a,
      bottom_right.x, bottom_right.y, bottom_right.z, bottom_right.r, bottom_right.g, bottom_right.b,
      bottom_right.a,
      bottom_left.x, bottom_left.y, bottom_left.z, bottom_left.r, bottom_left.g, bottom_left.b, bottom_left.a
  };

  unsigned short indices[] = {
      0, 1, 3,
      1, 3, 2
  };

  vertex_buffer_->Update(&positions[0]);
  index_buffer_->Update(&indices[0]);
}

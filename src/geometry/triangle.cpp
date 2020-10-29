//
// Created by artyomd on 1/6/20.
//

#include "src/geometry/triangle.hpp"

geometry::Triangle::Triangle(
    const std::shared_ptr<api::RenderingContext> &context,
    const geometry::Point &point_0,
    const geometry::Point &point_1,
    const geometry::Point &point_2) : GeometryItem(context) {

  api::VertexBufferLayout layout;
  layout.Push<float>(3);
  layout.Push<float>(4);
  vertex_buffer_ = context->CreateVertexBuffer(3 * 7 * sizeof(float), layout);
  index_buffer_ = context->CreateIndexBuffer(3, api::DataType::DATA_TYPE_UINT_16);

  Point geometry_data[] = {point_0, point_1, point_2};

  unsigned short indices[] = {
      0, 1, 2
  };

  vertex_buffer_->Update(&geometry_data[0]);
  index_buffer_->Update(&indices[0]);
}

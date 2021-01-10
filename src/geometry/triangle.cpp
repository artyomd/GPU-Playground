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
  size_t stride = sizeof(float) * 7;
  layout.Push({api::DataType::DATA_TYPE_FLOAT, 3, 0, stride});
  layout.Push({api::DataType::DATA_TYPE_FLOAT, 4, sizeof(float) * 3, stride});
  vertex_buffer_ = context->CreateVertexBuffer(3 * 7 * sizeof(float), layout);
  index_buffer_ = context->CreateIndexBuffer(3, api::DataType::DATA_TYPE_UINT_16);

  Point geometry_data[] = {point_0, point_1, point_2};
  vertex_buffer_->Update(&geometry_data[0]);

  unsigned short indices[] = {
      0, 1, 2
  };

  index_buffer_->Update(&indices[0]);
}

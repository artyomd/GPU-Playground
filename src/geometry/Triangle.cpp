//
// Created by artyomd on 1/6/20.
//

#include "src/geometry/Triangle.h"

geometry::Triangle::Triangle(
    std::shared_ptr<api::RenderingContext> context,
    geometry::Point &point_0,
    geometry::Point &point_1,
    geometry::Point &point_2) : GeometryItem(context) {

  std::vector<Point> geometry_data;
  geometry_data.push_back(point_2);
  geometry_data.push_back(point_1);
  geometry_data.push_back(point_0);

  unsigned short indices[] = {
      0, 1, 2
  };

  vertex_buffer_ = context->CreateVertexBuffer(geometry_data.data(), 3 * 7 * sizeof(float));
  layout_ = std::make_shared<api::VertexBufferLayout>();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(indices, 3, api::DataType::DATA_TYPE_UINT_16);
}

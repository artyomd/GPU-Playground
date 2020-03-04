//
// Created by artyomd on 1/6/20.
//

#include "Triangle.h"

geometry::Triangle::Triangle(api::RenderingContext *context, geometry::Point &point_0, geometry::Point &point_1,
                             geometry::Point &point_2) : GeometryItem(context) {
  float positions[] = {
      point_0.x, point_0.y, point_0.z, point_0.r, point_0.g, point_0.b, point_0.a,
      point_1.x, point_1.y, point_1.z, point_1.r, point_1.g, point_1.b, point_1.a,
      point_2.x, point_2.y, point_2.z, point_2.r, point_2.g, point_2.b, point_2.a
  };

  unsigned int indices[] = {
      0, 1, 2
  };

  vertex_buffer_ = context->CreateVertexBuffer(positions, 3*7*sizeof(float));
  layout_ = new api::VertexBufferLayout();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(indices, 3, api::DATA_TYPE_UINT_16);
}

geometry::Triangle::~Triangle() {
  context_->FreeIndexBuffer(index_buffer_);
  context_->FreeVertexBiding(vertex_binding_);
  delete (layout_);
  context_->FreeVertexBuffer(vertex_buffer_);
}

//
// Created by artyomd on 1/6/20.
//

#include "Triangle.h"

geometry::Triangle::Triangle(api::RenderingContext *context, geometry::Point &point_0, geometry::Point &point_1,
                             geometry::Point &point_2) : GeometryItem(context) {
  float positions[] = {
      point_0.x_, point_0.y_, point_0.z_, point_0.r_, point_0.g_, point_0.b_, point_0.a_,
      point_1.x_, point_1.y_, point_1.z_, point_1.r_, point_1.g_, point_1.b_, point_1.a_,
      point_2.x_, point_2.y_, point_2.z_, point_2.r_, point_2.g_, point_2.b_, point_2.a_
  };

  unsigned short indices[] = {
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

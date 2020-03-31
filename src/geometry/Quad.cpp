//
// Created by artyomd on 1/6/20.
//

#include "Quad.h"

geometry::Quad::Quad(api::RenderingContext *context, geometry::Point &top_left, geometry::Point &top_right,
                     geometry::Point &bottom_right, geometry::Point &bottom_left) : GeometryItem(context) {
  float positions[] = {
      top_left.x_, top_left.y_, top_left.z_, top_left.r_, top_left.g_, top_left.b_, top_left.a_,
      top_right.x_, top_right.y_, top_right.z_, top_right.r_, top_right.g_, top_right.b_, top_right.a_,
      bottom_right.x_, bottom_right.y_, bottom_right.z_, bottom_right.r_, bottom_right.g_, bottom_right.b_,
      bottom_right.a_,
      bottom_left.x_, bottom_left.y_, bottom_left.z_, bottom_left.r_, bottom_left.g_, bottom_left.b_, bottom_left.a_
  };

  unsigned short indices[] = {
      3, 2, 0,
      2, 1, 0
  };

  vertex_buffer_ = context->CreateVertexBuffer(positions, 4*7*sizeof(float));
  layout_ = new api::VertexBufferLayout();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(indices, 6, api::DATA_TYPE_UINT_16);
}

geometry::Quad::~Quad() {
  context_->FreeIndexBuffer(index_buffer_);
  context_->FreeVertexBiding(vertex_binding_);
  delete (layout_);
  context_->FreeVertexBuffer(vertex_buffer_);
}

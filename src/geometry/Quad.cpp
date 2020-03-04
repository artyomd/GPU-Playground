//
// Created by artyomd on 1/6/20.
//

#include "Quad.h"

geometry::Quad::Quad(api::RenderingContext *context, geometry::Point &top_left, geometry::Point &top_right,
                     geometry::Point &bottom_right, geometry::Point &bottom_left) : GeometryItem(context) {
  float positions[] = {
      top_left.x, top_left.y, top_left.z, top_left.r, top_left.g, top_left.b, top_left.a,
      top_right.x, top_right.y, top_right.z, top_right.r, top_right.g, top_right.b, top_right.a,
      bottom_right.x, bottom_right.y, bottom_right.z, bottom_right.r, bottom_right.g, bottom_right.b, bottom_right.a,
      bottom_left.x, bottom_left.y, bottom_left.z, bottom_left.r, bottom_left.g, bottom_left.b, bottom_left.a
  };

  unsigned char indices[] = {
      0, 1, 2,
      2, 3, 0
  };

  vertex_buffer_ = context->CreateVertexBuffer(positions, 4*7*sizeof(float));
  layout_ = new api::VertexBufferLayout();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(indices, 6, api::DATA_TYPE_BYTE);
}

geometry::Quad::~Quad() {
  context_->FreeIndexBuffer(index_buffer_);
  context_->FreeVertexBiding(vertex_binding_);
  delete (layout_);
  context_->FreeVertexBuffer(vertex_buffer_);
}

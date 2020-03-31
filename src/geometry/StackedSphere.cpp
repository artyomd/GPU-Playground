//
// Created by artyomd on 3/23/20.
//

#include "StackedSphere.h"
#include "Point.hpp"
geometry::StackedSphere::StackedSphere(api::RenderingContext *context,
                                       float radius,
                                       unsigned int stacks,
                                       unsigned int slices)
    : GeometryItem(context) {

  std::vector<Point> geometry_data;
  std::vector<unsigned short> index_data;

  for (unsigned int stack_number = 0; stack_number <= stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number < slices; ++slice_number) {
      auto theta = static_cast<float>(stack_number*M_PI/stacks);
      auto phi = static_cast<float>(slice_number*2*M_PI/slices);
      float sin_theta = std::sin(theta);
      float sin_phi = std::sin(phi);
      float cos_theta = std::cos(theta);
      float cos_phi = std::cos(phi);
      Point point = {radius*cos_phi*sin_theta, radius*sin_phi*sin_theta, radius*cos_theta,
                     1.0f, 0.0f, 0.0f, 1.0f};
      geometry_data.push_back(point);
    }
  }
  for (unsigned int stack_number = 0; stack_number < stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number <= slices; ++slice_number) {
      index_data.push_back(static_cast<unsigned short &&>((stack_number*slices) + (slice_number%slices)));
      index_data.push_back(static_cast<unsigned short &&>(((stack_number + 1)*slices) + (slice_number%slices)));
    }
  }
  vertex_buffer_ = context->CreateVertexBuffer(&geometry_data[0], 7*geometry_data.size()*sizeof(float));
  layout_ = new api::VertexBufferLayout();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(&index_data[0], index_data.size(), api::DATA_TYPE_UINT_16);

}
geometry::StackedSphere::~StackedSphere() {
  context_->FreeIndexBuffer(index_buffer_);
  context_->FreeVertexBiding(vertex_binding_);
  delete (layout_);
  context_->FreeVertexBuffer(vertex_buffer_);
}

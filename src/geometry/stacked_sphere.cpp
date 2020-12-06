//
// Created by artyomd on 3/23/20.
//

#include "src/geometry/stacked_sphere.hpp"

#include <cmath>

#include "src/geometry/point.hpp"

geometry::StackedSphere::StackedSphere(const std::shared_ptr<api::RenderingContext> &context,
                                       float radius,
                                       unsigned int stacks,
                                       unsigned int slices)
    : GeometryItem(context) {

  std::vector<Point> geometry_data;
  std::vector<unsigned short> index_data;

  for (unsigned int stack_number = 0; stack_number <= stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number < slices; ++slice_number) {
      auto theta = static_cast<float>(stack_number * M_PI / stacks);
      auto phi = static_cast<float>(slice_number * 2 * M_PI / slices);
      float sin_theta = std::sin(theta);
      float sin_phi = std::sin(phi);
      float cos_theta = std::cos(theta);
      float cos_phi = std::cos(phi);
      Point point = {radius * cos_phi * sin_theta, radius * sin_phi * sin_theta, radius * cos_theta,
                     1.0f, 0.0f, 0.0f, 1.0f};
      geometry_data.push_back(point);
    }
  }
  for (unsigned int stack_number = 0; stack_number < stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number <= slices; ++slice_number) {
      index_data.push_back(static_cast<unsigned short &&>((stack_number * slices) + (slice_number % slices)));
      index_data.push_back(static_cast<unsigned short &&>(((stack_number + 1) * slices) + (slice_number % slices)));
    }
  }

  api::VertexBufferLayout layout;
  size_t stride = sizeof(float) * 7;
  layout.Push({api::DataType::DATA_TYPE_FLOAT, 3, 0, stride});
  layout.Push({api::DataType::DATA_TYPE_FLOAT, 4, sizeof(float) * 3, stride});
  vertex_buffer_ = context->CreateVertexBuffer(7 * geometry_data.size() * sizeof(float), layout);
  vertex_buffer_->Update(&geometry_data[0]);

  index_buffer_ = context->CreateIndexBuffer(index_data.size(), api::DataType::DATA_TYPE_UINT_16);
  index_buffer_->Update(&index_data[0]);

}

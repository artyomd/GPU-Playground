#include "src/geometry/stacked_sphere.hpp"

#include <cmath>

#include "src/geometry/point.hpp"

geometry::StackedSphere::StackedSphere(
    const std::shared_ptr<api::RenderingContext> &context, float radius,
    unsigned int stacks, unsigned int slices)
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
      Point point = {radius * cos_phi * sin_theta,
                     radius * sin_phi * sin_theta,
                     radius * cos_theta,
                     1.0f,
                     0.0f,
                     0.0f,
                     1.0f};
      geometry_data.push_back(point);
    }
  }
  for (unsigned int stack_number = 0; stack_number < stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number <= slices;
         ++slice_number) {
      index_data.push_back(static_cast<unsigned short &&>(
                               (stack_number * slices) + (slice_number % slices)));
      index_data.push_back(static_cast<unsigned short &&>(
                               ((stack_number + 1) * slices) + (slice_number % slices)));
    }
  }

  vbl_.Push({0, api::DataType::FLOAT, 3});
  vbl_.Push({1, api::DataType::FLOAT, 4});
  vertex_buffer_ = context->CreateBuffer(
      7 * geometry_data.size() * sizeof(float), api::BufferUsage::VERTEX_BUFFER,
      api::MemoryType::DEVICE_LOCAL);
  vertex_buffer_->Update(&geometry_data[0]);

  index_buffer_data_type_ = api::DataType::UINT_16;
  index_buffer_ = context->CreateBuffer(
      index_data.size() * sizeof(unsigned short),
      api::BufferUsage::INDEX_BUFFER, api::MemoryType::DEVICE_LOCAL);
  index_count_ = index_data.size();
  index_buffer_->Update(&index_data[0]);
}

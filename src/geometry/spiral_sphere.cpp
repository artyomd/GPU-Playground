//
// Created by artyomd on 3/22/20.
//

#include "src/geometry/spiral_sphere.hpp"

#include <cmath>

#include "src/geometry/point.hpp"

geometry::SpiralSphere::SpiralSphere(
    const std::shared_ptr<api::RenderingContext> &context, float radius,
    unsigned int loops, unsigned int segments_per_loop)
    : GeometryItem(context) {
  std::vector<Point> geometry_data;
  std::vector<unsigned short> index_data;

  for (unsigned int loop_segment_number = 0;
       loop_segment_number < segments_per_loop; ++loop_segment_number) {
    float theta = 0;
    auto phi =
        static_cast<float>(loop_segment_number * 2 * M_PI / segments_per_loop);
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
                   0.0f};
    geometry_data.push_back(point);
  }
  for (unsigned int loop_number = 0; loop_number <= loops; ++loop_number) {
    for (unsigned int loop_segment_number = 0;
         loop_segment_number < segments_per_loop; ++loop_segment_number) {
      auto theta = static_cast<float>(
          (loop_number * M_PI / loops) +
          ((M_PI * loop_segment_number) / (segments_per_loop * loops)));
      if (loop_number == loops) {
        theta = static_cast<float>(M_PI);
      }
      auto phi = static_cast<float>(loop_segment_number * 2 * M_PI /
                                    segments_per_loop);
      float sin_theta = std::sin(theta);
      float sin_phi = std::sin(phi);
      float cos_theta = std::cos(theta);
      float cos_phi = std::cos(phi);
      Point point = {radius * cos_phi * sin_theta,
                     radius * sin_phi * sin_theta,
                     radius * cos_theta,
                     1.0f,
                     1.0f,
                     1.0f,
                     1.0f};
      geometry_data.push_back(point);
    }
  }
  for (unsigned int loop_segment_number = 0;
       loop_segment_number < segments_per_loop; ++loop_segment_number) {
    index_data.push_back(static_cast<unsigned short &&>(loop_segment_number));
    index_data.push_back(static_cast<unsigned short &&>(segments_per_loop +
                                                        loop_segment_number));
  }
  for (unsigned int loop_number = 0; loop_number < loops; ++loop_number) {
    for (unsigned int loop_segment_number = 0;
         loop_segment_number < segments_per_loop; ++loop_segment_number) {
      index_data.push_back(static_cast<unsigned short &&>(
          ((loop_number + 1) * segments_per_loop) + loop_segment_number));
      index_data.push_back(static_cast<unsigned short &&>(
          ((loop_number + 2) * segments_per_loop) + loop_segment_number));
    }
  }

  vbl_.Push({0, api::DataType::FLOAT, 3});
  vbl_.Push({1, api::DataType::FLOAT, 4});
  vertex_buffer_ = context->CreateBuffer(
      7 * geometry_data.size() * sizeof(float), api::BufferUsage::VERTEX_BUFFER,
      api::MemoryType::DEVICE_LOCAL);
  vertex_buffer_->Update(geometry_data.data());

  index_buffer_data_type_ = api::DataType::UINT_16;
  index_buffer_ = context->CreateBuffer(
      index_data.size() * sizeof(unsigned short),
      api::BufferUsage::INDEX_BUFFER, api::MemoryType::DEVICE_LOCAL);
  index_count_ = index_data.size();
  index_buffer_->Update(&index_data[0]);
}

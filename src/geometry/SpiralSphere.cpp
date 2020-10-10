//
// Created by artyomd on 3/22/20.
//

#include "src/geometry/SpiralSphere.h"

#include <cmath>

#include "src/geometry/Point.hpp"

geometry::SpiralSphere::SpiralSphere(
    std::shared_ptr<api::RenderingContext> context,
    float radius,
    unsigned int loops,
    unsigned int segments_per_loop)
    : GeometryItem(context) {

  std::vector<Point> geometry_data;
  std::vector<unsigned short> index_data;

  for (unsigned int loop_segment_number = 0; loop_segment_number < segments_per_loop; ++loop_segment_number) {
    float theta = 0;
    auto phi = static_cast<float>(loop_segment_number * 2 * M_PI / segments_per_loop);
    float sin_theta = std::sin(theta);
    float sin_phi = std::sin(phi);
    float cos_theta = std::cos(theta);
    float cos_phi = std::cos(phi);
    Point point = {radius * cos_phi * sin_theta, radius * sin_phi * sin_theta, radius * cos_theta, 1.0f, 0.0f, 0.0f,
                   0.0f};
    geometry_data.push_back(point);

  }
  for (unsigned int loop_number = 0; loop_number <= loops; ++loop_number) {
    for (unsigned int loop_segment_number = 0; loop_segment_number < segments_per_loop; ++loop_segment_number) {
      auto theta = static_cast<float>((loop_number * M_PI / loops) +
          ((M_PI * loop_segment_number) / (segments_per_loop * loops)));
      if (loop_number == loops) {
        theta = static_cast<float>(M_PI);
      }
      auto phi = static_cast<float>(loop_segment_number * 2 * M_PI / segments_per_loop);
      float sin_theta = std::sin(theta);
      float sin_phi = std::sin(phi);
      float cos_theta = std::cos(theta);
      float cos_phi = std::cos(phi);
      Point point = {radius * cos_phi * sin_theta, radius * sin_phi * sin_theta, radius * cos_theta, 1.0f, 1.0f, 1.0f,
                     1.0f};
      geometry_data.push_back(point);

    }
  }
  for (unsigned int loop_segment_number = 0; loop_segment_number < segments_per_loop; ++loop_segment_number) {
    index_data.push_back(static_cast<unsigned short &&>(loop_segment_number));
    index_data.push_back(static_cast<unsigned short &&>(segments_per_loop + loop_segment_number));
  }
  for (unsigned int loop_number = 0; loop_number < loops; ++loop_number) {
    for (unsigned int loop_segment_number = 0; loop_segment_number < segments_per_loop; ++loop_segment_number) {
      index_data.push_back(
          static_cast<unsigned short &&>(((loop_number + 1) * segments_per_loop) + loop_segment_number));
      index_data.push_back(
          static_cast<unsigned short &&>(((loop_number + 2) * segments_per_loop) + loop_segment_number));
    }
  }

  vertex_buffer_ = context->CreateVertexBuffer(geometry_data.data(), 7 * geometry_data.size() * sizeof(float));
  layout_ = std::make_shared<api::VertexBufferLayout>();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(index_data.data(), index_data.size(), api::DataType::DATA_TYPE_UINT_16);
}

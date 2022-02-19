#include "src/geometry/triangle.hpp"

geometry::Triangle::Triangle(
    const std::shared_ptr<api::RenderingContext> &context,
    const geometry::Point &point_0, const geometry::Point &point_1,
    const geometry::Point &point_2)
    : GeometryItem(context) {
  vbl_.Push({0, api::DataType::FLOAT, 3});
  vbl_.Push({1, api::DataType::FLOAT, 4});
  vertex_buffer_ = context->CreateBuffer(3 * 7 * sizeof(float),
                                         api::BufferUsage::VERTEX_BUFFER,
                                         api::MemoryType::DEVICE_LOCAL);
  index_buffer_ = context->CreateBuffer(3 * sizeof(unsigned short),
                                        api::BufferUsage::INDEX_BUFFER,
                                        api::MemoryType::DEVICE_LOCAL);
  index_buffer_data_type_ = api::DataType::UINT_16;

  Point geometry_data[] = {point_0, point_1, point_2};
  vertex_buffer_->Update(&geometry_data[0]);

  unsigned short indices[] = {0, 1, 2};
  index_count_ = 3;
  index_buffer_->Update(&indices[0]);
}

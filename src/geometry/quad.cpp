#include "src/geometry/quad.hpp"

geometry::Quad::Quad(const std::shared_ptr<api::RenderingContext> &context,
                     geometry::Point &top_left, geometry::Point &top_right,
                     geometry::Point &bottom_right,
                     geometry::Point &bottom_left)
    : GeometryItem(context) {
  vbl_.Push({0, api::DataType::FLOAT, 3});
  vbl_.Push({1, api::DataType::FLOAT, 4});
  vertex_buffer_ = context->CreateBuffer(4 * 7 * sizeof(float),
                                         api::BufferUsage::VERTEX_BUFFER,
                                         api::MemoryType::DEVICE_LOCAL);
  index_buffer_ = context->CreateBuffer(6 * sizeof(unsigned short),
                                        api::BufferUsage::INDEX_BUFFER,
                                        api::MemoryType::DEVICE_LOCAL);
  index_buffer_data_type_ = api::DataType::UINT_16;

  float positions[] = {
      top_left.x, top_left.y, top_left.z, top_left.r,
      top_left.g, top_left.b, top_left.a, top_right.x,
      top_right.y, top_right.z, top_right.r, top_right.g,
      top_right.b, top_right.a, bottom_right.x, bottom_right.y,
      bottom_right.z, bottom_right.r, bottom_right.g, bottom_right.b,
      bottom_right.a, bottom_left.x, bottom_left.y, bottom_left.z,
      bottom_left.r, bottom_left.g, bottom_left.b, bottom_left.a};
  vertex_buffer_->Update(&positions[0]);

  unsigned short indices[] = {0, 1, 3, 1, 3, 2};
  index_count_ = 6;
  index_buffer_->Update(&indices[0]);
}

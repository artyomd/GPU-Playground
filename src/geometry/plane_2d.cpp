#include "plane_2d.hpp"

#include <cassert>

#include "point.hpp"
#include "vulkan/utils.hpp"

geometry::Plane2d::Plane2d(const std::shared_ptr<vulkan::RenderingContext> &context, const uint32_t &width,
                           const uint32_t &height)
    : GeometryItem(context) {
  vertex_buffer_ = vulkan::Buffer::Create(context_, 3 * 7u * sizeof(float),
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

  std::vector<Point> vertices;
  std::vector<uint32_t> indices;

  for (auto y = 0u; y <= height; ++y) {
    for (auto x = 0u; x <= width; ++x) {
      vertices.emplace_back(x, 0.0, y);
    }
  }

  assert(vertices.size() <= std::numeric_limits<uint32_t>::max());

  for (auto y = 0u; y < height; ++y) {
    for (auto x = 0u; x < width; ++x) {
      unsigned int bottomLeft = (y * (width + 1)) + x;
      unsigned int bottomRight = bottomLeft + 1;
      unsigned int topLeft = bottomLeft + width + 1;
      unsigned int topRight = topLeft + 1;

      indices.push_back(bottomLeft);
      indices.push_back(topLeft);
      indices.push_back(bottomRight);
      indices.push_back(bottomRight);
      indices.push_back(topLeft);
      indices.push_back(topRight);
    }
  }

  vbl_.Push({.binding_index = 0, .type = vulkan::DataType::FLOAT, .count = 3});
  vertex_buffer_ = vulkan::Buffer::Create(context_, vertices.size() * sizeof(decltype(vertices)::value_type),
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
  index_buffer_ = vulkan::Buffer::Create(context_, indices.size() * sizeof(decltype(indices)::value_type),
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

  auto *command_pool = context_->CreateCommandPool();
  {
    const auto staging_buffer =
        vulkan::Buffer::Create(context_, vertex_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), vertices.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto *command_buffer = context_->CreateCommandBuffer(command_pool);
    constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    vertex_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto *fence = context_->CreateFence();
    context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context_->WaitForFence(fence);
    context_->DestroyFence(fence);
    context_->FreeCommandBuffer(command_pool, command_buffer);
  }
  {
    index_count_ = indices.size();
    index_buffer_data_type_ = VK_INDEX_TYPE_UINT16;

    const auto staging_buffer =
        vulkan::Buffer::Create(context_, index_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), indices.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto *command_buffer = context_->CreateCommandBuffer(command_pool);
    constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    index_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto *fence = context_->CreateFence();
    context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context_->WaitForFence(fence);
    context_->DestroyFence(fence);
    context_->FreeCommandBuffer(command_pool, command_buffer);
  }
  context_->DestroyCommandPool(command_pool);
}

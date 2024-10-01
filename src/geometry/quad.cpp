#include "geometry/quad.hpp"

#include <array>
#include <cstring>

#include "vulkan/utils.hpp"

geometry::Quad::Quad(const std::shared_ptr<vulkan::RenderingContext> &context, const Point &point_0,
                     const Point &point_1, const Point &point_2, const Point &point_3)
    : GeometryItem(context) {
  const std::array geometry_data = {point_0, point_1, point_2, point_3};
  const std::array<uint16_t, 6> indices = {0, 1, 3, 1, 3, 2};
  vbl_.Push({.binding_index = 0, .type = vulkan::DataType::FLOAT, .count = 3});
  vertex_buffer_ = vulkan::Buffer::Create(context_, geometry_data.size() * sizeof(decltype(geometry_data)::value_type),
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
  index_buffer_ = vulkan::Buffer::Create(context_, indices.size() * sizeof(decltype(indices)::value_type),
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

  auto *command_pool = context_->CreateCommandPool();
  {
    const auto staging_buffer =
        vulkan::Buffer::Create(context_, vertex_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), geometry_data.data(), staging_buffer->GetSizeInBytes());
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

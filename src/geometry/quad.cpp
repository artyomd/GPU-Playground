#include "geometry/quad.hpp"

#include "vulkan/utils.hpp"

#include <array>
#include <cstring>

geometry::Quad::Quad(
    const std::shared_ptr<vulkan::RenderingContext> &context,
    const geometry::Point &point_0, const geometry::Point &point_1,
    const geometry::Point &point_2, const geometry::Point &point_3)
    : GeometryItem(context) {
  vbl_.Push({0, vulkan::DataType::FLOAT, 3});
  vbl_.Push({1, vulkan::DataType::FLOAT, 4});
  vertex_buffer_ = vulkan::Buffer::Create(context_,
                                          4 * 7 * sizeof(float),
                                          VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                                              | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          false);
  index_buffer_ = vulkan::Buffer::Create(context_,
                                         6 * sizeof(uint16_t),
                                         VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                                             | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                         false);

  auto command_pool = context_->CreateCommandPool();
  {
    std::array<Point, 4> geometry_data = {point_0, point_1, point_2, point_3};
    auto staging_buffer = vulkan::Buffer::Create(context_,
                                                 vertex_buffer_->GetSizeInBytes(),
                                                 VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), geometry_data.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto command_buffer = context_->CreateCommandBuffer(command_pool);
    VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    vertex_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto fence = context_->CreateFence();
    context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context_->WaitForFence(fence);
    context_->DestroyFence(fence);
    context_->FreeCommandBuffer(command_pool, command_buffer);
  }
  {
    std::array<uint16_t, 6> indices = {0, 1, 3, 1, 3, 2};
    index_count_ = indices.size();
    index_buffer_data_type_ = VkIndexType::VK_INDEX_TYPE_UINT16;

    auto staging_buffer = vulkan::Buffer::Create(context_,
                                                 index_buffer_->GetSizeInBytes(),
                                                 VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), indices.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto command_buffer = context_->CreateCommandBuffer(command_pool);
    VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    index_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto fence = context_->CreateFence();
    context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context_->WaitForFence(fence);
    context_->DestroyFence(fence);
    context_->FreeCommandBuffer(command_pool, command_buffer);
  }
  context_->DestroyCommandPool(command_pool);
}

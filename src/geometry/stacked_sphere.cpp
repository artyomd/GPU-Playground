#include "stacked_sphere.hpp"

#include "point.hpp"
#include "vulkan/utils.hpp"

#include <cmath>
#include <cstring>

geometry::StackedSphere::StackedSphere(
    const std::shared_ptr<vulkan::RenderingContext> &context, float radius,
    unsigned int stacks, unsigned int slices)
    : GeometryItem(context) {
  std::vector<Point> geometry_data;
  std::vector<uint16_t> index_data;

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
      index_data.push_back(static_cast<uint16_t>((stack_number * slices) + (slice_number % slices)));
      index_data.push_back(static_cast<uint16_t>(((stack_number + 1) * slices) + (slice_number % slices)));
    }
  }

  vbl_.Push({0, vulkan::DataType::FLOAT, 3});
  vbl_.Push({1, vulkan::DataType::FLOAT, 4});

  vertex_buffer_ = vulkan::Buffer::Create(context_,
                                          7 * geometry_data.size() * sizeof(float),
                                          VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                                              | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          false);
  index_buffer_ = vulkan::Buffer::Create(context_,
                                         index_data.size() * sizeof(uint16_t),
                                         VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                                             | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                         false);

  auto command_pool = context_->CreateCommandPool();
  {
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
    auto staging_buffer = vulkan::Buffer::Create(context_,
                                                 index_buffer_->GetSizeInBytes(),
                                                 VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), index_data.data(), staging_buffer->GetSizeInBytes());
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

  index_count_ = index_data.size();
  index_buffer_data_type_ = VK_INDEX_TYPE_UINT16;
}
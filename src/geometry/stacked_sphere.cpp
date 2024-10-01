#include "stacked_sphere.hpp"

#include <cassert>
#include <cmath>
#include <cstring>

#include "point.hpp"
#include "vulkan/utils.hpp"

geometry::StackedSphere::StackedSphere(const std::shared_ptr<vulkan::RenderingContext> &context, const float radius,
                                       const unsigned int stacks, const unsigned int slices)
    : GeometryItem(context) {
  assert(slices > 0);
  std::vector<PointWithColor> geometry_data;
  std::vector<uint16_t> index_data;

  for (unsigned int stack_number = 0; stack_number <= stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number < slices; ++slice_number) {
      const auto theta = static_cast<float>(stack_number * M_PI / stacks);
      const auto phi = static_cast<float>(slice_number * 2 * M_PI / slices);
      const float sin_theta = std::sin(theta);
      const float sin_phi = std::sin(phi);
      const float cos_theta = std::cos(theta);
      const float cos_phi = std::cos(phi);
      PointWithColor point = {
          radius * cos_phi * sin_theta, radius * sin_phi * sin_theta, radius * cos_theta, 255, 0, 0, 255};
      geometry_data.push_back(point);
    }
  }
  for (unsigned int stack_number = 0; stack_number < stacks; ++stack_number) {
    for (unsigned int slice_number = 0; slice_number <= slices; ++slice_number) {
      index_data.push_back(static_cast<uint16_t>((stack_number * slices) + (slice_number % slices)));
      index_data.push_back(static_cast<uint16_t>(((stack_number + 1) * slices) + (slice_number % slices)));
    }
  }

  vbl_.Push({.binding_index = 0, .type = vulkan::DataType::FLOAT, .count = 3});
  vbl_.Push({.binding_index = 1, .type = vulkan::DataType::UINT_8, .count = 4});

  vertex_buffer_ = vulkan::Buffer::Create(context_, geometry_data.size() * sizeof(decltype(geometry_data)::value_type),
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
  index_buffer_ = vulkan::Buffer::Create(context_, index_data.size() * sizeof(decltype(index_data)::value_type),
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
    const auto staging_buffer =
        vulkan::Buffer::Create(context_, index_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), index_data.data(), staging_buffer->GetSizeInBytes());
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

  index_count_ = index_data.size();
  index_buffer_data_type_ = VK_INDEX_TYPE_UINT16;
}

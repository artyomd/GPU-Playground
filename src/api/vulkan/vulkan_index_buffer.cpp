//
// Created by artyomd on 2/25/20.
//

#include "src/api/vulkan/vulkan_index_buffer.hpp"

#include <cstring>
#include <stdexcept>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanIndexBuffer::VulkanIndexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                  unsigned int item_count,
                                                  enum DataType type)
    : IndexBuffer(item_count, type),
      context_(std::move(context)),
      device_(context_->GetDevice()) {
  index_type_ = GetVkType(type);

  context_->CreateBuffer(size_in_bytes_,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         &buffer_,
                         &memory_);
}

void api::vulkan::VulkanIndexBuffer::Update(void *data) {
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  context_->CreateBuffer(size_in_bytes_,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &staging_buffer,
                         &staging_buffer_memory);

  void *mapped_data;
  vkMapMemory(device_, staging_buffer_memory, 0, size_in_bytes_, 0, &mapped_data);
  memcpy(mapped_data, data, (size_t) size_in_bytes_);
  vkUnmapMemory(device_, staging_buffer_memory);
  context_->CopyBuffer(staging_buffer, buffer_, size_in_bytes_);
  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

const VkBuffer *api::vulkan::VulkanIndexBuffer::GetBuffer() const {
  return &buffer_;
}

VkIndexType api::vulkan::VulkanIndexBuffer::GetIndexType() const {
  return index_type_;
}

api::vulkan::VulkanIndexBuffer::~VulkanIndexBuffer() {
  context_->WaitForGpuIdle();
  vkDestroyBuffer(device_, buffer_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

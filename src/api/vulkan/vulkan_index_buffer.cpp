//
// Created by artyomd on 2/25/20.
//

#include "src/api/vulkan/vulkan_index_buffer.hpp"

#include <cstring>
#include <stdexcept>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanIndexBuffer::VulkanIndexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                  const void *indices_data,
                                                  unsigned int item_count,
                                                  enum DataType type)
    : IndexBuffer(item_count, type), context_(std::move(context)), device_(context_->GetDevice()) {
  if (type != DataType::DATA_TYPE_UINT_16 && type != DataType::DATA_TYPE_UINT_32) {
    throw std::runtime_error("unsupported type for index buffer");
  }
  index_type_ = GetVkType(type);

  VkDeviceSize buffer_size = GetDataTypeSizeInBytes(type) * item_count;
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  context_->CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &staging_buffer,
                         &staging_buffer_memory);

  void *data;
  vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, indices_data, (size_t) buffer_size);
  vkUnmapMemory(device_, staging_buffer_memory);

  context_->CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         &buffer_,
                         &memory_);
  context_->CopyBuffer(staging_buffer, buffer_, buffer_size);
  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void api::vulkan::VulkanIndexBuffer::Bind() const {
  vkCmdBindIndexBuffer(context_->GetCurrentCommandBuffer(), buffer_, 0, this->index_type_);
}

void api::vulkan::VulkanIndexBuffer::Unbind() const {
}

api::vulkan::VulkanIndexBuffer::~VulkanIndexBuffer() {
  context_->WaitForGpuIdle();
  vkDestroyBuffer(device_, buffer_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

//
// Created by artyomd on 12/30/19.
//

#include "src/api/vulkan/vulkan_vertex_buffer.hpp"

#include <cstring>

api::vulkan::VulkanVertexBuffer::VulkanVertexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                    const void *vertices_data,
                                                    VkDeviceSize buffer_size)
    : context_(std::move(context)), device_(context_->GetDevice()) {
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  context_->CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &staging_buffer,
                         &staging_buffer_memory);

  void *data;
  vkMapMemory(device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, vertices_data, (size_t) buffer_size);
  vkUnmapMemory(device_, staging_buffer_memory);

  context_->CreateBuffer(buffer_size,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         &buffer_,
                         &memory_);
  context_->CopyBuffer(staging_buffer, buffer_, buffer_size);
  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

void api::vulkan::VulkanVertexBuffer::Bind() const {
  VkBuffer vertex_buffers[] = {buffer_};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(context_->GetCurrentCommandBuffer(), 0, 1, vertex_buffers, offsets);
}

void api::vulkan::VulkanVertexBuffer::Unbind() const {

}

api::vulkan::VulkanVertexBuffer::~VulkanVertexBuffer() {
  vkDestroyBuffer(device_, buffer_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

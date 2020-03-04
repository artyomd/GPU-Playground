//
// Created by artyomd on 12/30/19.
//

#include <cstring>
#include "VkVertexBuffer.h"

namespace api {
VkVertexBuffer::VkVertexBuffer(VkRenderingContext *context, const void *vertices_data, VkDeviceSize buffer_size)
    : device_(context->GetDevice()), context_(context) {
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  context->CreateBuffer(buffer_size,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        staging_buffer,
                        staging_buffer_memory);

  void *data;
  vkMapMemory(*device_, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, vertices_data, (size_t) buffer_size);
  vkUnmapMemory(*device_, staging_buffer_memory);

  context->CreateBuffer(buffer_size,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        buffer_,
                        memory_);
  context->CopyBuffer(staging_buffer, buffer_, buffer_size);
  vkDestroyBuffer(*device_, staging_buffer, nullptr);
  vkFreeMemory(*device_, staging_buffer_memory, nullptr);
}

void VkVertexBuffer::Bind() const {
  VkBuffer vertex_buffers[] = {buffer_};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(*context_->GetCurrentCommandBuffer(), 0, 1, vertex_buffers, offsets);
}

void VkVertexBuffer::Unbind() const {

}

VkVertexBuffer::~VkVertexBuffer() {
  vkDestroyBuffer(*device_, buffer_, nullptr);
  vkFreeMemory(*device_, memory_, nullptr);
}
}
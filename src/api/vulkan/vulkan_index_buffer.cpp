//
// Created by artyomd on 2/25/20.
//

#include "src/api/vulkan/vulkan_index_buffer.hpp"

#include "src/api/vulkan/vulkan_utils.hpp"
#include "vulkan_buffer.hpp"

api::vulkan::VulkanIndexBuffer::VulkanIndexBuffer(const std::shared_ptr<VulkanRenderingContext> &context,
                                                  uint32_t item_count,
                                                  enum DataType type)
    : Buffer(item_count * GetDataTypeSizeInBytes(type)),
      VulkanBuffer(context, item_count * GetDataTypeSizeInBytes(type),
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      IndexBuffer(item_count, type) {
  index_type_ = GetVkType(type);
}

VkIndexType api::vulkan::VulkanIndexBuffer::GetIndexType() const {
  return index_type_;
}

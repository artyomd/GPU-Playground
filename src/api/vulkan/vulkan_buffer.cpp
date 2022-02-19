#include "vulkan_buffer.hpp"

#include <cstring>

#include "vulkan_utils.hpp"

api::vulkan::VulkanBuffer::VulkanBuffer(const std::shared_ptr<VulkanRenderingContext> &context,
                                        const size_t &length,
                                        VkBufferUsageFlags usage,
                                        VkMemoryPropertyFlags properties)
    : api::Buffer(length),
      context_(context),
      device_(context->GetDevice()),
      host_visible_(!(properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
  if (!host_visible_) {
    usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  }
  context->CreateBuffer(length, usage, properties, &buffer_, &memory_);
}

void api::vulkan::VulkanBuffer::Update(const void *data) {
  if (host_visible_) {
    void *mapped_data = nullptr;
    CheckVkResult(vkMapMemory(device_, memory_, 0, size_in_bytes_, 0, &mapped_data));
    memcpy(mapped_data, data, size_in_bytes_);
    vkUnmapMemory(device_, memory_);
  } else {
    VulkanBuffer tmp_buffer(this->context_,
                            this->size_in_bytes_,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            GetVkMemoryType(MemoryType::HOST_VISIBLE));
    tmp_buffer.Update(data);
    context_->CopyBuffer(tmp_buffer.GetBuffer(), buffer_, size_in_bytes_);
  }
}

void api::vulkan::VulkanBuffer::CopyFrom(std::shared_ptr<api::Buffer> src_buffer,
                                         size_t size,
                                         size_t src_offset,
                                         size_t dst_offset) {
  context_->CopyBuffer(std::dynamic_pointer_cast<api::vulkan::VulkanBuffer>(src_buffer)->GetBuffer(),
                       buffer_,
                       size,
                       src_offset,
                       dst_offset);
}

VkBuffer api::vulkan::VulkanBuffer::GetBuffer() const {
  return buffer_;
}

api::vulkan::VulkanBuffer::~VulkanBuffer() {
  vkDestroyBuffer(device_, buffer_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

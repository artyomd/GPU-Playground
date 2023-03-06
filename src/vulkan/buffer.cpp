#include "buffer.hpp"

#include "utils.hpp"

#include <cstring>

std::shared_ptr<vulkan::Buffer> vulkan::Buffer::Create(const std::shared_ptr<RenderingContext> &context,
                                                       const size_t &length,
                                                       VkBufferUsageFlags usage,
                                                       bool host_visible) {
  return std::shared_ptr<vulkan::Buffer>(new vulkan::Buffer(context, length, usage, host_visible));
}

vulkan::Buffer::Buffer(const std::shared_ptr<RenderingContext> &context,
                       const size_t &length,
                       VkBufferUsageFlags usage,
                       bool host_visible)
    : context_(context),
      allocator_(context_->GetAllocator()),
      size_in_bytes_(length),
      host_visible_(host_visible) {

  VkBufferCreateInfo buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size_in_bytes_,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VmaAllocationCreateInfo alloc_info = {
      .flags = static_cast<VmaAllocationCreateFlags>(host_visible_
                                                     ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT : 0),
      .usage = host_visible_ ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
  };

  VK_CALL(vmaCreateBuffer(allocator_, &buffer_info, &alloc_info, &buffer_, &allocation_, nullptr));
}

void *vulkan::Buffer::Map() {
  if (!host_visible_) {
    throw std::runtime_error("only supported on host visible textures");
  }
  void *mapped_data = nullptr;
  VK_CALL(vmaMapMemory(allocator_, allocation_, &mapped_data));
  return mapped_data;
}

void vulkan::Buffer::Unmap() {
  if (!host_visible_) {
    throw std::runtime_error("only supported on host visible textures");
  }
  vmaUnmapMemory(allocator_, allocation_);
}

void vulkan::Buffer::CopyFrom(VkCommandBuffer command_buffer,
                              const std::shared_ptr<Buffer> &src_buffer,
                              size_t size,
                              size_t src_offset,
                              size_t dst_offset) {
  VkBufferCopy copy_region = {
      .srcOffset = src_offset,
      .dstOffset = dst_offset,
      .size = size,
  };
  vkCmdCopyBuffer(command_buffer, src_buffer->buffer_, buffer_, 1, &copy_region);
}

VkBuffer vulkan::Buffer::GetBuffer() const {
  return buffer_;
}

size_t vulkan::Buffer::GetSizeInBytes() const {
  return size_in_bytes_;
}

vulkan::Buffer::~Buffer() {
  vmaDestroyBuffer(allocator_, buffer_, allocation_);
}
VkWriteDescriptorSet vulkan::Buffer::GetWriteDescriptorSetFor(unsigned int binding_point) const {
  throw std::runtime_error("not implemented");
//  VkWriteDescriptorSet write_descriptor_set{
//      .dstBinding = binding_point,
//      .dstArrayElement = 0,
//      .descriptorCount = 1,
//      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//      .pBufferInfo = &buffer,
//  };
//  return write_descriptor_set;
}

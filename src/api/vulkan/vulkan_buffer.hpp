#pragma once

#include <cstddef>
#include "src/api/buffer.hpp"
#include "vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanBuffer : virtual public Buffer {
 public:
  VulkanBuffer() = delete;
  VulkanBuffer(const VulkanBuffer &) = delete;
  VulkanBuffer(const std::shared_ptr<VulkanRenderingContext> &context,
               const size_t &length,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties);
  void Update(const void *data) override;
  void CopyFrom(std::shared_ptr<api::Buffer> src_buffer, size_t size, size_t src_offset, size_t dst_offset) override;
  [[nodiscard]] VkBuffer GetBuffer() const;
  ~VulkanBuffer() override;
 protected:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkBuffer buffer_ = nullptr;
  VkDeviceMemory memory_ = nullptr;
 private:
  bool host_visible_;
};
}

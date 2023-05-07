#pragma once

#include "rendering_context.hpp"

#include <cstddef>

namespace vulkan {
class Buffer {
 public:
  static std::shared_ptr<Buffer> Create(const std::shared_ptr<RenderingContext> &context,
                                        const size_t &length,
                                        VkBufferUsageFlags usage,
                                        bool host_visible);
  Buffer() = delete;
  Buffer(const Buffer &) = delete;
  [[nodiscard]] void *Map();
  void Unmap();
  void CopyFrom(VkCommandBuffer command_buffer,
                const std::shared_ptr<Buffer> &src_buffer,
                size_t size,
                size_t src_offset = 0,
                size_t dst_offset = 0);
  [[nodiscard]] VkBuffer GetBuffer() const;
  [[nodiscard]] size_t GetSizeInBytes() const;
  virtual ~Buffer();
 private:
  Buffer(const std::shared_ptr<RenderingContext> &context,
         const size_t &length,
         VkBufferUsageFlags usage,
         bool host_visible);

  std::shared_ptr<RenderingContext> context_;
  VmaAllocator allocator_ = nullptr;
  size_t size_in_bytes_;
  VkBuffer buffer_ = nullptr;
  VmaAllocation allocation_ = nullptr;
  bool host_visible_;
};
}  // namespace vulkan
#pragma once

#include <cstddef>

#include "rendering_context.hpp"

namespace vulkan {
class Buffer final {
 public:
  static std::shared_ptr<Buffer> Create(const std::shared_ptr<RenderingContext> &context, const size_t &length,
                                        VkBufferUsageFlags usage, bool host_visible);
  Buffer() = delete;
  Buffer(const Buffer &) = delete;
  Buffer(Buffer &&) = delete;
  Buffer &operator=(const Buffer &) = delete;
  Buffer &operator=(Buffer &&) = delete;

  [[nodiscard]] void *Map() const;
  void Unmap() const;
  void CopyFrom(const VkCommandBuffer &command_buffer, const std::shared_ptr<Buffer> &src_buffer, size_t size,
                size_t src_offset = 0, size_t dst_offset = 0) const;
  [[nodiscard]] VkBuffer GetBuffer() const;
  [[nodiscard]] size_t GetSizeInBytes() const;
  ~Buffer();

 private:
  Buffer(const std::shared_ptr<RenderingContext> &context, const size_t &length, VkBufferUsageFlags usage,
         bool host_visible);

  std::shared_ptr<RenderingContext> context_;
  VmaAllocator allocator_ = nullptr;
  size_t size_in_bytes_;
  VkBuffer buffer_ = nullptr;
  VmaAllocation allocation_ = nullptr;
  bool host_visible_;
};
}  // namespace vulkan

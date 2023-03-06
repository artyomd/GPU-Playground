#pragma once

#include <cstddef>

#include "rendering_context.hpp"
#include "buffer.hpp"
#include "sampler.hpp"

namespace vulkan {
class Image {
 public:
  static std::shared_ptr<Image> Create(const std::shared_ptr<RenderingContext> &context,
                                       uint32_t width,
                                       uint32_t height,
                                       VkFormat pixel_format,
                                       VkImageUsageFlags usage,
                                       bool host_visible,
                                       VkSampleCountFlagBits sample_count = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

  static std::shared_ptr<Image> Create(const std::shared_ptr<RenderingContext> &context,
                                       uint32_t width,
                                       uint32_t height,
                                       VkFormat pixel_format,
                                       VkImage image);

  Image() = delete;
  Image(const Image &) = delete;

  void BlitFrom(VkCommandBuffer command_buffer, const std::shared_ptr<vulkan::Image> &other);
  void CopyFrom(VkCommandBuffer command_buffer, const std::shared_ptr<vulkan::Image> &other);
  void CopyFrom(VkCommandBuffer command_buffer, const std::shared_ptr<vulkan::Buffer> &other);
  void CopyTo(VkCommandBuffer command_buffer, const std::shared_ptr<vulkan::Buffer> &other);
  void TransferTo(VkCommandBuffer command_buffer, VkImageLayout new_layout);

  [[nodiscard]] void *Map();
  void Unmap();

  [[nodiscard]] size_t GetSizeInBytes() const;

  [[nodiscard]] uint32_t GetWidth() const { return width_; }

  [[nodiscard]] uint32_t GetHeight() const { return height_; }

  [[nodiscard]] VkFormat GetPixelFormat() const { return pixel_format_; }

  [[nodiscard]] VkImage GetImage() const;

  virtual ~Image();
 private:
  Image(const std::shared_ptr<RenderingContext> &context,
        uint32_t width,
        uint32_t height,
        VkFormat pixel_format,
        VkImageUsageFlags usage,
        bool host_visible,
        VkSampleCountFlagBits sample_count = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);

  Image(const std::shared_ptr<RenderingContext> &context,
        uint32_t width,
        uint32_t height,
        VkFormat pixel_format,
        VkImage image);

  std::shared_ptr<RenderingContext> context_;
  uint32_t width_;
  uint32_t height_;
  VkFormat pixel_format_;

  VkImage image_ = VK_NULL_HANDLE;
  VmaAllocation allocation_ = nullptr;
  VkImageLayout current_layout_ = VK_IMAGE_LAYOUT_UNDEFINED;
  bool host_visible_ = false;
};
}  // namespace vulkan
#pragma once

#include "vulkan_include.hpp"

#include "rendering_context.hpp"

namespace vulkan {
class RenderPass {
 public:
  explicit RenderPass(std::shared_ptr<RenderingContext> rendering_context,
                      VkFormat color_attachment_format,
                      VkFormat depth_attachment_format = VK_FORMAT_UNDEFINED,
                      VkSampleCountFlagBits color_attachment_sample_count = VK_SAMPLE_COUNT_1_BIT);
  [[nodiscard]] VkFormat GetColorAttachmentFormat() const;
  [[nodiscard]] VkFormat GetDepthAttachmentFormat() const;
  [[nodiscard]] VkSampleCountFlagBits GetSampleCount() const;
  [[nodiscard]] VkRenderPass GetRenderPass() const;
  virtual ~RenderPass();
 private:
  std::shared_ptr<RenderingContext> rendering_context_ = nullptr;
  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  VkFormat color_attachment_format_ = VK_FORMAT_UNDEFINED;
  VkFormat depth_attachment_format_ = VK_FORMAT_UNDEFINED;
  VkSampleCountFlagBits sample_count_ = VK_SAMPLE_COUNT_1_BIT;
};
}  // namespace vulkan
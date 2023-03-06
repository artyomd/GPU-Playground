#pragma once

#include "image_view.hpp"
#include "render_pass.hpp"
#include "rendering_context.hpp"

#include <vector>

namespace vulkan {
class Framebuffer {
 public:
  static std::shared_ptr<Framebuffer> Create(std::shared_ptr<RenderingContext> rendering_context,
                                             std::shared_ptr<RenderPass> render_pass,
                                             std::vector<std::shared_ptr<ImageView>> attachments);
  Framebuffer() = delete;
  Framebuffer(const Framebuffer &) = delete;

  [[nodiscard]] VkFramebuffer GetFramebuffer() const;
  [[nodiscard]] uint32_t GetWidth() const;
  [[nodiscard]] uint32_t GetHeight() const;

  std::shared_ptr<ImageView> GetDepthAttachment();

  virtual ~Framebuffer();
 private:
  Framebuffer(std::shared_ptr<RenderingContext> rendering_context,
              std::shared_ptr<RenderPass> render_pass,
              std::vector<std::shared_ptr<ImageView>> attachments);

  std::shared_ptr<RenderingContext> rendering_context_ = nullptr;
  std::shared_ptr<RenderPass> render_pass_ = nullptr;

  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;

  std::vector<std::shared_ptr<ImageView>> attachments_;
};
}  // namespace vulkan

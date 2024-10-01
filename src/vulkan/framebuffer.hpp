#pragma once

#include <vector>

#include "image_view.hpp"
#include "render_pass.hpp"
#include "rendering_context.hpp"

namespace vulkan {
class Framebuffer final {
 public:
  static std::shared_ptr<Framebuffer> Create(const std::shared_ptr<RenderingContext>& rendering_context,
                                             const std::shared_ptr<RenderPass>& render_pass,
                                             const std::vector<std::shared_ptr<ImageView>>& attachments);
  Framebuffer() = delete;
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&&) = delete;

  Framebuffer& operator=(const Framebuffer&) = delete;
  Framebuffer& operator=(Framebuffer&&) = delete;

  [[nodiscard]] VkFramebuffer GetFramebuffer() const;
  [[nodiscard]] uint32_t GetWidth() const;
  [[nodiscard]] uint32_t GetHeight() const;

  [[nodiscard]] std::shared_ptr<ImageView> GetDepthAttachment() const;

  ~Framebuffer();

 private:
  Framebuffer(const std::shared_ptr<RenderingContext>& rendering_context,
              const std::shared_ptr<RenderPass>& render_pass,
              const std::vector<std::shared_ptr<ImageView>>& attachments);

  std::shared_ptr<RenderingContext> rendering_context_ = nullptr;
  std::shared_ptr<RenderPass> render_pass_ = nullptr;

  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;

  std::vector<std::shared_ptr<ImageView>> attachments_;
};
}  // namespace vulkan

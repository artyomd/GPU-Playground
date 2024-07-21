#pragma once

#include "vulkan/rendering_context.hpp"
#include "vulkan/render_pass.hpp"

namespace renderable {
class ImguiWrapper {
 private:
  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  uint32_t image_count_ = 0;

 public:
  ImguiWrapper(std::shared_ptr<vulkan::RenderingContext> context,
               std::shared_ptr<vulkan::RenderPass> render_pass,
               uint32_t image_count,
               VkSampleCountFlagBits sample_count);
  [[nodiscard]] uint32_t GetImageCount() const;
  virtual ~ImguiWrapper();
};
} // namespace renderable
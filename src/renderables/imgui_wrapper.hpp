#pragma once

#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"

namespace renderable {
class ImguiWrapper final {
  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  uint32_t image_count_ = 0;

 public:
  ImguiWrapper() = delete;
  ImguiWrapper(const std::shared_ptr<vulkan::RenderingContext>& context,
               const std::shared_ptr<vulkan::RenderPass>& render_pass, const uint32_t& image_count,
               const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT);
  ImguiWrapper(const ImguiWrapper&) = delete;
  ImguiWrapper(ImguiWrapper&&) = delete;
  ImguiWrapper& operator=(const ImguiWrapper&) = delete;
  ImguiWrapper& operator=(ImguiWrapper&&) = delete;
  [[nodiscard]] uint32_t GetImageCount() const;
  static void ShowFPSOverlay();
  static void BeginFrame();
  static void EndFrameAndDraw(const VkCommandBuffer& cmd_buffer);
  ~ImguiWrapper();
};
}  // namespace renderable

#pragma once

#include "application/renderable.hpp"
#include "renderables/imgui_wrapper.hpp"
#include "menu.hpp"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"

#include <map>

namespace renderable {
class DefaultMenu
    : public application::Renderable {
 private:
  struct ImageContext {
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
  };

  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::weak_ptr<Menu> parent_;

  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  std::shared_ptr<ImguiWrapper> imgui_wrapper_ = nullptr;

  std::map<std::shared_ptr<vulkan::Image>, ImageContext> command_buffers_;
  std::map<std::shared_ptr<vulkan::Image>, std::shared_ptr<vulkan::Framebuffer>> framebuffers_;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  void CleanupCommandBuffers();

  explicit DefaultMenu(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
 public:
  static std::shared_ptr<DefaultMenu> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                             std::shared_ptr<Menu> parent);
  DefaultMenu() = delete;
  DefaultMenu(const DefaultMenu &) = delete;
  void SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) override;
  VkSemaphore Render(std::shared_ptr<vulkan::Image> image, const VkSemaphore &semaphore) override;
  ~DefaultMenu() override;
};
}


#pragma once

#include "application/renderable.hpp"
#include "geometry/gltf_model.hpp"
#include "renderables/imgui_wrapper.hpp"
#include "renderables/menu/menu.hpp"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"

#include <map>

namespace renderable {
class GltfModel : public application::Renderable {
 private:
  struct ImageContext {
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    uint32_t descriptor_index = 0;
  };

  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::weak_ptr<Menu> parent_;

  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  std::shared_ptr<ImguiWrapper> imgui_wrapper_ = nullptr;

  std::map<std::shared_ptr<vulkan::Image>, ImageContext> command_buffers_;
  std::map<std::shared_ptr<vulkan::Image>, std::shared_ptr<vulkan::Framebuffer>> framebuffers_;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;

  std::shared_ptr<geometry::GltfModel> model_ = nullptr;
  std::vector<std::string> scenes_names_;
  int current_scene_ = -1;
  int selected_scene_ = -1;

  void CleanupCommandBuffers();
  GltfModel(std::shared_ptr<vulkan::RenderingContext> context,
            std::shared_ptr<Menu> parent);
 public:
  static std::shared_ptr<GltfModel> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                           std::shared_ptr<Menu> parent);
  GltfModel() = delete;
  GltfModel(const GltfModel &) = delete;
  void SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) override;
  VkSemaphore Render(std::shared_ptr<vulkan::Image> image, const VkSemaphore &semaphore) override;
  ~GltfModel() override;
};
} // namespace renderable


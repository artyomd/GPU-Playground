#pragma once

#include "application/renderable.hpp"
#include "geometry/quad.hpp"
#include "imgui_wrapper.hpp"
#include "renderables/menu/menu.hpp"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"
#include "vulkan/rendering_pipeline.hpp"

#include <glm/glm.hpp>

#include <map>

namespace renderable {
class Shader : public application::Renderable {
 private:
  struct UniformBufferObjectShader {
    alignas(8) float screen_width = 0;
    alignas(4) float screen_height = 0;
    alignas(4) float time = 0;
  };
  UniformBufferObjectShader uniform_buffer_object_mvp_{};
  std::shared_ptr<geometry::Quad> quad_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  struct ImageContext {
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    std::shared_ptr<vulkan::Buffer> uniform_buffer = nullptr;
    uint32_t descriptor_index = 0;
  };

  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::weak_ptr<Menu> parent_;

  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  std::shared_ptr<ImguiWrapper> imgui_wrapper_ = nullptr;

  std::map<std::shared_ptr<vulkan::Image>, ImageContext> command_buffers_;
  std::map<std::shared_ptr<vulkan::Image>, std::shared_ptr<vulkan::Framebuffer>> framebuffers_;

  std::shared_ptr<vulkan::RenderingPipeline> pipeline_ = nullptr;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  void CleanupCommandBuffers();
 protected:
  Shader(std::shared_ptr<vulkan::RenderingContext> context,
         std::shared_ptr<Menu> parent,
         std::string fragment_shader_path);
 public:
  static std::shared_ptr<Shader> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                        std::shared_ptr<Menu> parent,
                                        std::string fragment_shader_path);
  Shader() = delete;
  Shader(const Shader &) = delete;
  void SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) override;
  VkSemaphore Render(std::shared_ptr<vulkan::Image> image, const VkSemaphore &semaphore) override;
  ~Shader() override;
};
} // namespace renderable


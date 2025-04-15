#pragma once

#include <chrono>
#include <glm/glm.hpp>
#include <map>

#include "application/renderable.hpp"
#include "renderables/imgui_wrapper.hpp"
#include "renderables/menu/menu.hpp"
#include "vulkan/compute_pipeline.hpp"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"
#include "vulkan/rendering_pipeline.hpp"

namespace renderable {
class Particles final : public application::Renderable {
  struct UniformBufferObject {
    alignas(16) uint32_t delta_time = 1.0f;
  };
  struct ImageContext {
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    uint32_t descriptor_index = 0;
    std::shared_ptr<vulkan::Buffer> uniform_buffer = nullptr;
    std::shared_ptr<vulkan::Buffer> vertex_ssbo = nullptr;
    std::shared_ptr<vulkan::Buffer> velocity_ssbo = nullptr;
  };

  std::shared_ptr<vulkan::Buffer> initial_positions_buffer_ = nullptr;
  std::shared_ptr<vulkan::Buffer> initial_velocity_buffer_ = nullptr;
  uint32_t particle_count_ = 8192;

  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::weak_ptr<Menu> parent_;

  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  std::shared_ptr<ImguiWrapper> imgui_wrapper_ = nullptr;

  std::shared_ptr<vulkan::Buffer> last_vertex_buffer_ = nullptr;
  std::shared_ptr<vulkan::Buffer> last_velocity_buffer_ = nullptr;
  // compute pipeline
  std::map<std::shared_ptr<vulkan::Image>, ImageContext> command_buffers_;
  std::shared_ptr<vulkan::Shader> c_shader_ = nullptr;
  UniformBufferObject ubo_{};
  std::shared_ptr<vulkan::ComputePipeline> compute_pipeline_ = nullptr;

  // render pipeline
  std::map<uint32_t, vulkan::VertexBufferLayout> vbl_;
  std::shared_ptr<vulkan::Buffer> color_buffer_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;
  std::shared_ptr<vulkan::RenderingPipeline> render_pipline_ = nullptr;
  std::map<std::shared_ptr<vulkan::Image>, std::shared_ptr<vulkan::Framebuffer>> framebuffers_;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;

  std::chrono::system_clock::time_point last_draw_time_ = std::chrono::system_clock::now();

  void CleanupCommandBuffers();
  Particles(const std::shared_ptr<vulkan::RenderingContext>& context, const std::shared_ptr<Menu>& parent);

 public:
  static std::shared_ptr<Particles> Create(const std::shared_ptr<vulkan::RenderingContext>& context,
                                           const std::shared_ptr<Menu>& parent);
  Particles() = delete;
  Particles(const Particles&) = delete;
  Particles(Particles&&) = delete;
  Particles& operator=(const Particles&) = delete;
  Particles& operator=(Particles&&) = delete;
  void SetupImages(const std::vector<std::shared_ptr<vulkan::Image>>& images) override;
  void Render(const std::shared_ptr<vulkan::Image>& image, const VkSemaphore& wait_semaphore,
              const VkSemaphore& signal_semaphore) override;
  ~Particles() override;
};
}  // namespace renderable

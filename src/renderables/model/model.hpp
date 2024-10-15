#pragma once

#include <map>

#include "application/renderable.hpp"
#include "glm/glm.hpp"
#include "renderables/imgui_wrapper.hpp"
#include "renderables/menu/menu.hpp"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"
#include "vulkan/rendering_pipeline.hpp"

namespace renderable {
class Model : public application::Renderable {
  struct UniformBufferObjectMvp {
    alignas(16) glm::mat4 model = glm::mat4(1.0F);
    alignas(16) glm::mat4 view = glm::mat4(1.0F);
    alignas(16) glm::mat4 proj = glm::mat4(1.0F);
  };
  glm::vec3 translation_ = glm::vec3(0.0F, 0.0F, 0.0F);
  glm::vec3 scale_ = glm::vec3(1.0F, 1.0F, 1.0F);
  glm::vec3 rotate_ = glm::vec3(0.0F, 0.0F, 0.0F);

  glm::vec3 eye_ = glm::vec3(2.0, 2.0, 0.0);
  glm::vec3 center_ = glm::vec3(0.0, 0.0, 0.0);

  float projection_width_ = 4.0;
  float projection_height_ = 0;
  UniformBufferObjectMvp uniform_buffer_object_mvp_{};

  struct ImageContext {
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

  bool add_depth_attachment_ = false;
  bool use_perspective_projection_ = false;

  void CleanupCommandBuffers();

 protected:
  void WaitForCommandBuffersToFinish() const;
  virtual glm::mat4 ComputeViewMatrix();
  Model(const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent,
        bool add_depth_attachment = false, bool use_perspective_projection = false);
  virtual std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(
      const std::shared_ptr<vulkan::RenderingContext> &rendering_context,
      const std::shared_ptr<vulkan::RenderPass> &render_pass, const size_t &descriptor_set_count,
      const VkSampleCountFlagBits &sample_count) = 0;
  virtual size_t NumOfIndicesToDraw() = 0;
  void SetLookAt(const glm::vec3 &eye, const glm::vec3 &center);

 public:
  Model() = delete;
  Model(const Model &) = delete;
  Model(Model &&) = delete;
  Model &operator=(const Model &) = delete;
  Model &operator=(Model &&) = delete;

  void SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) override;
  void Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &waitSemaphore,
              const VkSemaphore &signalSemaphore) override;
  ~Model() override;
};
}  // namespace renderable


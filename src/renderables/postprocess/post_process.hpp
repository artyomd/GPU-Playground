#pragma once

// clang-format off
#include <imgui.h>
#include <imfilebrowser.h>
// clang-format on

#include <glm/glm.hpp>

#include "application/renderable.hpp"
#include "greyscale.hpp"
#include "renderables/imgui_wrapper.hpp"
#include "renderables/menu/menu.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_pipeline.hpp"
#include "vulkan/shader.hpp"

namespace renderable {
class PostProcess final : public application::Renderable {
  struct UniformBufferObject {
    alignas(16) glm::mat4 model = glm::mat4(1.0F);
    alignas(16) glm::mat4 view = glm::mat4(1.0F);
    alignas(16) glm::mat4 proj = glm::mat4(1.0F);
  };
  UniformBufferObject ubo_{};
  glm::vec2 window_size_{};
  glm::vec2 image_size_{};
  struct ImageContext {
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    std::shared_ptr<vulkan::Buffer> uniform_buffer = nullptr;
    uint32_t descriptor_index = 0;
    // processed image info
    std::uint64_t params_hash = 0;
    std::shared_ptr<vulkan::ImageView> processed_image = nullptr;
  };

  std::string image_file_ = RESOURCE_DIR + std::string("textures/texture2d.jpg");
  ImGui::FileBrowser file_dialog_ = ImGui::FileBrowser(ImGuiFileBrowserFlags_NoModal);

  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::weak_ptr<Menu> parent_;

  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  std::shared_ptr<ImguiWrapper> imgui_wrapper_ = nullptr;

  std::map<std::shared_ptr<vulkan::Image>, ImageContext> command_buffers_;

  // post-process pipeline
  std::shared_ptr<vulkan::ImageView> source_image_ = nullptr;
  std::shared_ptr<Blur> blur_ = nullptr;

  // render pipeline
  std::shared_ptr<vulkan::Sampler> sampler_ = nullptr;
  vulkan::VertexBufferLayout vbl_;
  std::shared_ptr<vulkan::Buffer> vertex_buffer_ = nullptr;
  std::shared_ptr<vulkan::Buffer> index_buffer_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;
  std::shared_ptr<vulkan::RenderingPipeline> pipeline_ = nullptr;
  std::map<std::shared_ptr<vulkan::Image>, std::shared_ptr<vulkan::Framebuffer>> framebuffers_;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;

  void CleanupCommandBuffers();
  PostProcess(const std::shared_ptr<vulkan::RenderingContext>& context, const std::shared_ptr<Menu>& parent);
  [[nodiscard]] std::shared_ptr<vulkan::Image> LoadImage(const std::string& path) const;
  void UpdateImage();
  void UpdateModelMatrix();

 public:
  static std::shared_ptr<PostProcess> Create(const std::shared_ptr<vulkan::RenderingContext>& context,
                                             const std::shared_ptr<Menu>& parent);
  PostProcess() = delete;
  PostProcess(const PostProcess&) = delete;
  PostProcess(PostProcess&&) = delete;
  PostProcess& operator=(const PostProcess&) = delete;
  PostProcess& operator=(PostProcess&&) = delete;
  void SetupImages(const std::vector<std::shared_ptr<vulkan::Image>>& images) override;
  void Render(const std::shared_ptr<vulkan::Image>& image, const VkSemaphore& wait_semaphore,
              const VkSemaphore& signal_semaphore) override;
  ~PostProcess() override;
};
}  // namespace renderable

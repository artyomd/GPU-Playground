#pragma once

#include "model.hpp"

namespace renderable {
class ObjModel : public Model {
 private:
  vulkan::VertexBufferLayout vbl_{};
  std::shared_ptr<vulkan::Buffer> vertex_buffer_ = nullptr;
  size_t index_count_ = 0;
  VkIndexType index_type_ = VkIndexType::VK_INDEX_TYPE_NONE_KHR;
  std::shared_ptr<vulkan::Buffer> index_buffer_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  std::shared_ptr<vulkan::Sampler> sampler_ = nullptr;
  std::shared_ptr<vulkan::ImageView> image_view_ = nullptr;

  ObjModel(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                            std::shared_ptr<vulkan::RenderPass> render_pass,
                                                            size_t descriptor_set_count,
                                                            VkSampleCountFlagBits sample_count) override;
  size_t NumOfIndicesToDraw() override;
 protected:
  glm::mat4 ComputeViewMatrix() override;
 public:
  static std::shared_ptr<ObjModel> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                          std::shared_ptr<Menu> parent);
  ObjModel() = delete;
  ObjModel(const ObjModel &) = delete;
  ~ObjModel() override;
};
} // namespace renderable
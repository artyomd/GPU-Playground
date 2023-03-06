#pragma once

#include "geometry/quad.hpp"
#include "model.hpp"

namespace renderable {
class Texture2d : public Model {
 private:
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;
  std::shared_ptr<vulkan::Buffer> vertex_buffer_ = nullptr;
  vulkan::VertexBufferLayout vertex_buffer_layout_{};

  std::shared_ptr<vulkan::Buffer> index_buffer_ = nullptr;
  VkIndexType index_buffer_data_type_ = VkIndexType::VK_INDEX_TYPE_UINT16;
  size_t index_count_ = 0;

  std::shared_ptr<vulkan::Sampler> sampler_ = nullptr;
  std::shared_ptr<vulkan::ImageView> image_view_ = nullptr;

  Texture2d(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                            std::shared_ptr<vulkan::RenderPass> render_pass,
                                                            size_t descriptor_set_count,
                                                            VkSampleCountFlagBits sample_count) override;
  size_t NumOfIndicesToDraw() override;
 public:
  static std::shared_ptr<Texture2d> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                           std::shared_ptr<Menu> parent);
  Texture2d() = delete;
  Texture2d(const Texture2d &) = delete;
  ~Texture2d() override;
};
} // namespace renderable


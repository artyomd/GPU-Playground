#pragma once

#include "buffer.hpp"
#include "image_view.hpp"
#include "render_pass.hpp"
#include "rendering_context.hpp"
#include "sampler.hpp"
#include "shader.hpp"
#include "vertex_buffer_layout.hpp"

namespace vulkan {

struct RenderingPipelineConfig {
  VkPrimitiveTopology draw_mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  VkCullModeFlags cull_mode = VK_CULL_MODE_NONE;
  VkFrontFace front_face = VK_FRONT_FACE_CLOCKWISE;
  VkBool32 enable_depth_test = VK_FALSE;
  VkCompareOp depth_function = VK_COMPARE_OP_LESS;
  VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;
  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
      .blendEnable = VK_FALSE,
      .colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };
};

class RenderingPipeline final {
 public:
  static std::shared_ptr<RenderingPipeline> Create(const std::shared_ptr<RenderingContext> &context,
                                                   const std::shared_ptr<RenderPass> &render_pass,
                                                   const std::shared_ptr<Shader> &vertex_shader,
                                                   const std::shared_ptr<Shader> &fragment_shader,
                                                   const VertexBufferLayout &vbl, const RenderingPipelineConfig &config,
                                                   const size_t &descriptor_set_count);
  static std::shared_ptr<RenderingPipeline> Create(const std::shared_ptr<RenderingContext> &context,
                                                   const std::shared_ptr<RenderPass> &render_pass,
                                                   const std::shared_ptr<Shader> &vertex_shader,
                                                   const std::shared_ptr<Shader> &fragment_shader,
                                                   const std::map<uint32_t, VertexBufferLayout> &vbl_map,
                                                   const RenderingPipelineConfig &config,
                                                   const size_t &descriptor_set_count);

  RenderingPipeline() = delete;
  RenderingPipeline(const RenderingPipeline &) = delete;
  RenderingPipeline(RenderingPipeline &&) = delete;

  RenderingPipeline &operator=(const RenderingPipeline &) = delete;
  RenderingPipeline &operator=(RenderingPipeline &&) = delete;

  void SetVertexBuffer(const uint32_t &index, const std::shared_ptr<Buffer> &vertex_buffer);
  void SetIndexBuffer(const std::shared_ptr<Buffer> &buffer, VkIndexType element_type);
  void SetImageView(uint32_t binding_point, uint32_t descriptor_set_index, const std::shared_ptr<ImageView> &image_view,
                    const std::shared_ptr<Sampler> &sampler);
  void SetUniformBuffer(uint32_t binding_point, uint32_t descriptor_set_index,
                        const std::shared_ptr<Buffer> &uniform_buffer);

  [[nodiscard]] size_t GetDescriptorSetCount() const;

  void DrawIndexed(const VkCommandBuffer &command_buffer, const uint32_t &index_count,
                   const size_t &descriptor_index) const;

  void Draw(const VkCommandBuffer &command_buffer, const uint32_t &vertex_count, const size_t &descriptor_index) const;

  ~RenderingPipeline();

 private:
  RenderingPipeline(const std::shared_ptr<RenderingContext> &context, const std::shared_ptr<RenderPass> &render_pass,
                    const std::shared_ptr<Shader> &vertex_shader, const std::shared_ptr<Shader> &fragment_shader,
                    const std::map<uint32_t, VertexBufferLayout> &vbl_map, const RenderingPipelineConfig &config,
                    size_t descriptor_set_count);

  std::shared_ptr<RenderingContext> context_;
  std::shared_ptr<RenderPass> render_pass_;

  VkPipeline pipeline_ = VK_NULL_HANDLE;
  VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;

  std::map<uint32_t, std::shared_ptr<Buffer>> vertex_buffers_;

  std::shared_ptr<Buffer> index_buffer_ = nullptr;
  VkIndexType index_type_ = VK_INDEX_TYPE_UINT16;

  std::shared_ptr<Shader> vertex_shader_ = nullptr;
  std::shared_ptr<Shader> fragment_shader_ = nullptr;

  struct DescriptorSetInfo {
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    std::map<uint32_t, std::shared_ptr<ImageView>> image_views;
    std::map<uint32_t, std::shared_ptr<Buffer>> uniform_buffers;
  };
  std::vector<DescriptorSetInfo> descriptor_sets_;
  VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;
};
}  // namespace vulkan

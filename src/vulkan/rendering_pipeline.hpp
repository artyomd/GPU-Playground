#pragma once

#include "buffer.hpp"
#include "vertex_buffer_layout.hpp"
#include "image_view.hpp"
#include "render_pass.hpp"
#include "rendering_context.hpp"
#include "shader.hpp"

namespace vulkan {
struct RenderingPipelineConfig {
  VkPrimitiveTopology draw_mode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  VkCullModeFlags cull_mode = VK_CULL_MODE_NONE;
  VkFrontFace front_face = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
  VkBool32 enable_depth_test = VK_FALSE;
  VkCompareOp depth_function = VkCompareOp::VK_COMPARE_OP_LESS;
  VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;
};
class RenderingPipeline {
 public:
  static std::shared_ptr<vulkan::RenderingPipeline> Create(const std::shared_ptr<RenderingContext> &context,
                                                           const std::shared_ptr<RenderPass> &render_pass,
                                                           const std::shared_ptr<Shader> &vertex_shader,
                                                           const std::shared_ptr<Shader> &fragment_shader,
                                                           const VertexBufferLayout &vbl,
                                                           RenderingPipelineConfig config,
                                                           size_t descriptor_set_count);

  RenderingPipeline() = delete;
  RenderingPipeline(const Buffer &) = delete;

  void SetVertexBuffer(const std::shared_ptr<Buffer> &vertex_buffer);
  void SetIndexBuffer(const std::shared_ptr<Buffer> &buffer, VkIndexType element_type);
  void SetImageView(unsigned int binding_point,
                    unsigned int descriptor_set_index,
                    const std::shared_ptr<ImageView> &image_view,
                    const std::shared_ptr<Sampler> &sampler);
  void SetUniformBuffer(unsigned int binding_point,
                        unsigned int descriptor_set_index,
                        const std::shared_ptr<Buffer> &uniform_buffer);

  [[nodiscard]] size_t GetDescriptorSetCount();

  void Draw(VkCommandBuffer command_buffer,
            size_t index_count,
            size_t offset,
            size_t descriptor_index);

  ~RenderingPipeline();
 private:
  RenderingPipeline(const std::shared_ptr<RenderingContext> &context,
                    const std::shared_ptr<RenderPass> &render_pass,
                    const std::shared_ptr<Shader> &vertex_shader,
                    const std::shared_ptr<Shader> &fragment_shader,
                    const VertexBufferLayout &vbl,
                    RenderingPipelineConfig config,
                    size_t descriptor_set_count);

  std::shared_ptr<RenderingContext> context_;
  std::shared_ptr<RenderPass> render_pass_;

  VkPipeline pipeline_ = VK_NULL_HANDLE;
  VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;

  std::shared_ptr<Buffer> vertex_buffer_ = nullptr;

  std::shared_ptr<Buffer> index_buffer_ = nullptr;
  VkIndexType index_type_ = VkIndexType::VK_INDEX_TYPE_UINT16;

  std::shared_ptr<Shader> vertex_shader_ = nullptr;
  std::shared_ptr<Shader> fragment_shader_ = nullptr;

  struct DescriptorSetInfo {
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    std::map<unsigned int, std::shared_ptr<ImageView>> image_views;
    std::map<unsigned int, std::shared_ptr<Buffer>> uniform_buffers;
  };
  std::vector<DescriptorSetInfo> descriptor_sets_;
  VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;
};
}  // namespace vulkan

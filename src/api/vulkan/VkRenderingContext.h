//
// Created by artyomd on 12/31/19.
//

#pragma once

#include "src/api/RenderingContext.h"

namespace api {
class VkRenderingContext : public RenderingContext {
 private:
  VkDevice *device_;
  VkPhysicalDevice *physical_device_;
  VkCommandPool *graphics_pool_;
  VkQueue *graphics_queue_;
  VkDescriptorPool *descriptor_pool_;
  int image_count_;
  int current_image_index_ = 0;
  VkRenderPass *vk_render_pass_ = nullptr;
  VkExtent2D swap_chain_extent_{};
  VkCommandBuffer *current_command_buffer_ = nullptr;
  VkSampleCountFlagBits msaa_samples_;

  VkSampleCountFlagBits GetMaxUsableSampleCount();
 public:
  VkRenderingContext(VkPhysicalDevice *physical_device,
                     VkDevice *device,
                     VkCommandPool *graphics_pool,
                     VkQueue *graphics_queue,
                     VkDescriptorPool *descriptor_pool,
                     int image_count);

  [[nodiscard]] VkDevice *GetDevice() const;

  IndexBuffer *CreateIndexBuffer(const void *data, unsigned int size, DataType type) override;

  void FreeIndexBuffer(IndexBuffer *buffer) override;

  VertexBuffer *CreateVertexBuffer(const void *data, unsigned int size) override;

  void FreeVertexBuffer(VertexBuffer *buffer) override;

  VertexBinding *CreateVertexBinding(const VertexBuffer *buffer,
                                     const VertexBufferLayout *vertex_buffer_layout) override;

  void FreeVertexBiding(VertexBinding *vertex_binding) override;

  RenderingPipeline *CreateGraphicsPipeline(const VertexBinding *vertex_binding,
                                            const IndexBuffer *index_buffer,
                                            const Shader *vertex_shader,
                                            const Shader *fragment_shader,
                                            const RenderingPipelineLayout *pipeline_layout,
                                            const RenderingPipelineLayoutConfig &config = {}) override;

  void FreeGraphicsPipeline(RenderingPipeline *pipeline) override;

  RenderingPipelineLayout *CreateRenderingPipelineLayout(const std::vector<Uniform *> &bindings) override;
  void FreeRenderingPipelineLayout(RenderingPipelineLayout *pipeline_layout) override;

  Shader *CreateShader(std::string sipr_v_shader_location,
                       std::string glsl_location,
                       std::string entry_point_name,
                       api::ShaderType type) override;

  void DeleteShader(Shader *vertex_binding) override;

  UniformBuffer *CreateUniformBuffer(int length,
                                     int binding_point,
                                     ShaderType shader_stage) override;

  void DeleteUniformBuffer(UniformBuffer *uniform_buffer) override;

  void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

  void TransitionImageLayout(VkImage image,
                             VkImageLayout old_layout,
                             VkImageLayout new_layout);

  void CreateBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer &buffer,
                    VkDeviceMemory &buffer_memory);

  void CopyBuffer(VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize size);

  VkImageView CreateImageView(VkImage &image, VkFormat format, VkImageAspectFlagBits aspect_mask);

  VkCommandBuffer BeginSingleTimeCommands(VkCommandPool &command_pool);

  void EndSingleTimeCommands(VkQueue &queue, VkCommandPool &pool, VkCommandBuffer &command_buffer);
  uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;
  [[nodiscard]] VkRenderPass *GetVkRenderPass() const;
  [[nodiscard]] VkExtent2D GetSwapChainExtent() const;
  [[nodiscard]] VkCommandBuffer *GetCurrentCommandBuffer() const;
  void SetCurrentCommandBuffer(VkCommandBuffer *current_command_buffer);
  void SetVkRenderPass(VkRenderPass *vk_render_pass);
  void SetViewportSize(int width, int height) override;
  VkDescriptorPool *GetDescriptorPool();
  int GetImageCount();
  [[nodiscard]] int GetCurrentImageIndex() const;
  void SetCurrentImageIndex(int current_image_index);
  void WaitForGpuIdle() const override;
  ~VkRenderingContext() override = default;
  Texture2D *CreateTexture2D(std::string image_path, int binding_point, ShaderType shader_stage) override;
  void DeleteTexture2D(Texture2D *texture_2_d) override;
  [[nodiscard]] VkFormat FindDepthFormat() const;
  [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates,
                                             VkImageTiling tiling,
                                             VkFormatFeatureFlags features) const;
  void CreateImage(uint32_t width,
                   uint32_t height,
                   VkSampleCountFlagBits num_samples,
                   VkFormat format,
                   VkImageTiling tiling,
                   VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkImage &image,
                   VkDeviceMemory &image_memory) const;

  [[nodiscard]] VkSampleCountFlagBits GetMsaaSamples() const;
};
}
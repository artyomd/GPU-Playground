//
// Created by artyomd on 12/31/19.
//

#pragma once

#include "RenderingContext.h"

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

  uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

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
                                            const UniformBuffer *shader_properties) override;

  void FreeGraphicsPipeline(RenderingPipeline *pipeline) override;

  Shader *CreateShader(std::string sipr_v_shader_location,
                       std::string glsl_location,
                       std::string entry_point_name,
                       api::ShaderType type) override;

  void DeleteShader(Shader *vertex_binding) override;

  UniformBuffer *CreateUniformBuffer(int length,
                                     int binding_point,
                                     ShaderType shader_stage) override;

  void DeleteUniformBuffer(UniformBuffer *uniform_buffer) override;

  void CreateBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer &buffer,
                    VkDeviceMemory &buffer_memory);

  void CopyBuffer(VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize size);

  VkImageView CreateImageView(VkImage &image, VkFormat format);

  VkCommandBuffer BeginSingleTimeCommands(VkCommandPool &command_pool);

  void EndSingleTimeCommands(VkQueue &queue, VkCommandPool &pool, VkCommandBuffer &command_buffer);
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
};
}
//
// Created by artyomd on 12/31/19.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/rendering_context.hpp"

namespace api::vulkan {
class VulkanRenderingContext : public RenderingContext, public std::enable_shared_from_this<VulkanRenderingContext> {
  VkPhysicalDevice physical_device_;
  VkDevice device_;
  VkQueue graphics_queue_;
  VkCommandPool graphics_pool_;
  VkDescriptorPool descriptor_pool_;

  uint32_t image_count_;
  uint32_t current_image_index_ = 0;
  VkRenderPass vk_render_pass_ = nullptr;
  VkCommandBuffer current_command_buffer_ = nullptr;
  VkSampleCountFlagBits msaa_samples_;

  VkSampleCountFlagBits GetMaxUsableSampleCount();
 public:
  VulkanRenderingContext(VkPhysicalDevice physical_device,
                         VkDevice device,
                         VkQueue graphics_queue,
                         VkCommandPool graphics_pool,
                         VkDescriptorPool descriptor_pool,
                         uint32_t image_count);

  [[nodiscard]] VkDevice GetDevice() const;

  std::shared_ptr<Buffer> CreateBuffer(size_t size_in_bytes) override;

  std::shared_ptr<IndexBuffer> CreateIndexBuffer(uint32_t count, DataType type) override;

  std::shared_ptr<VertexBuffer> CreateVertexBuffer(size_t size_in_bytes, VertexBufferLayout layout) override;

  std::shared_ptr<RenderingPipeline> CreateGraphicsPipeline(
      std::shared_ptr<VertexBuffer> vertex_binding,
      std::shared_ptr<IndexBuffer> index_buffer,
      std::shared_ptr<Shader> vertex_shader,
      std::shared_ptr<Shader> fragment_shader,
      RenderingPipelineConfig config) override;

  std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_source,
                                       std::string entry_point_name,
                                       api::ShaderType type) override;

  void WaitForGpuIdle() const override;

  std::shared_ptr<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat pixel_format) override;

  ~VulkanRenderingContext() override = default;

  void CreateBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer *buffer,
                    VkDeviceMemory *buffer_memory);

  void CreateImage(uint32_t width,
                   uint32_t height,
                   VkSampleCountFlagBits num_samples,
                   VkFormat format,
                   VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkImage *image,
                   VkDeviceMemory *image_memory) const;

  void CopyBuffer(VkBuffer src_buffer,
                  VkBuffer dst_buffer,
                  VkDeviceSize size,
                  VkDeviceSize src_offset = 0,
                  VkDeviceSize dst_offset = 0);

  void CopyBufferToImage(VkBuffer buffer, VkImage image, size_t width, size_t height);

  void TransitionImageLayout(VkImage image,
                             VkImageLayout old_layout,
                             VkImageLayout new_layout);

  void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect_mask, VkImageView *image_view);

  VkCommandBuffer BeginSingleTimeCommands(VkCommandPool command_pool);

  void EndSingleTimeCommands(VkQueue queue, VkCommandPool pool, VkCommandBuffer command_buffer);

  [[nodiscard]] uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

  void SetVkRenderPass(VkRenderPass vk_render_pass);

  [[nodiscard]] VkRenderPass GetVkRenderPass() const;

  void SetCurrentCommandBuffer(VkCommandBuffer current_command_buffer);

  [[nodiscard]] VkCommandBuffer GetCurrentCommandBuffer() const;

  VkDescriptorPool GetDescriptorPool();

  [[nodiscard]] uint32_t GetImageCount() const;

  void SetCurrentImageIndex(uint32_t current_image_index);

  [[nodiscard]] uint32_t GetCurrentImageIndex() const;

  [[nodiscard]] VkFormat FindDepthFormat() const;

  [[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates,
                                             VkImageTiling tiling,
                                             VkFormatFeatureFlags features) const;

  [[nodiscard]] VkSampleCountFlagBits GetMsaaSamples() const;
};
}

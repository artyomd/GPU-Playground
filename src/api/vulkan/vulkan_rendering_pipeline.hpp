//
// Created by artyomd on 3/4/20.
//

#pragma once

#include <map>
#include <vulkan/vulkan.h>

#include "src/api/rendering_pipeline.hpp"
#include "src/api/vulkan/vulkan_index_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"
#include "src/api/vulkan/vulkan_shader.hpp"
#include "src/api/vulkan/vulkan_vertex_buffer.hpp"
#include "src/api/vulkan/vulkan_texture_2d.hpp"

namespace api::vulkan {
class VulkanRenderingPipeline : public RenderingPipeline {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  RenderingPipelineConfig config_;

  VkPipeline pipeline_{};
  VkPipelineLayout pipeline_layout_ = nullptr;
  VkViewport viewport_ = {};
  VkRect2D scissor_ = {};

  std::shared_ptr<VulkanVertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<VulkanIndexBuffer> index_buffer_ = nullptr;
  std::shared_ptr<VulkanShader> vertex_shader_ = nullptr;
  std::shared_ptr<VulkanShader> fragment_shader_ = nullptr;
  std::vector<std::shared_ptr<VulkanTexture2D>> textures_{};

  std::map<unsigned int, std::vector<std::shared_ptr<VulkanBuffer>>> uniform_buffers_;

  std::vector<VkDescriptorSet> descriptor_sets_{};
  VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;

  void CreateUniformBuffers(std::shared_ptr<VulkanShader> shader);
  void DestroyPipeline();
  void CreatePipeline();

 public:
  VulkanRenderingPipeline(std::shared_ptr<VulkanRenderingContext> context,
                          const std::shared_ptr<VertexBuffer> &vertex_buffer,
                          const std::shared_ptr<IndexBuffer> &index_buffer,
                          const std::shared_ptr<Shader> &vertex_shader,
                          const std::shared_ptr<Shader> &fragment_shader,
                          RenderingPipelineConfig config);

  void Render() override;
  void UpdateUniformBuffer(unsigned int binding_point, void *data) override;
  void SetTexture(unsigned int binding_point, std::shared_ptr<api::Texture2D> texture) override;
  void SetViewPort(size_t width, size_t height) override;
  ~VulkanRenderingPipeline() override;
};
}

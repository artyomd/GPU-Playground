//
// Created by artyomd on 3/8/20.
//

#pragma once

#include "src/api/uniform_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"
#include "src/api/vulkan/vulkan_uniform.hpp"

namespace api::vulkan {
class VulkanUniformBuffer : public UniformBuffer, public VulkanUniform {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  int length_;
  std::vector<VkBuffer> uniform_buffers_{};
  std::vector<VkDescriptorBufferInfo> descriptor_buffer_info_{};
  std::vector<VkDeviceMemory> uniform_buffers_memory_{};
 public:
  VulkanUniformBuffer(std::shared_ptr<VulkanRenderingContext> context,
                      int length,
                      int binding_point,
                      ShaderType shader_stage);
  void Update(const void *data) override;
  [[nodiscard]] VkDescriptorSetLayoutBinding GetLayoutBinding() const override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const override;
  ~VulkanUniformBuffer() override;
};
}
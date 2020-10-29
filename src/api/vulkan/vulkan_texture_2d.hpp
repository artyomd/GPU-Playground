//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/texture_2d.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"
#include "src/api/vulkan/vulkan_uniform.hpp"

namespace api::vulkan {
class VulkanTexture2D : public Texture2D, public VulkanUniform {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkDescriptorImageInfo image_info_{};
  VkImageView image_view_{};
  VkImage image_{};
  VkDeviceMemory image_memory_{};

 public:
  VulkanTexture2D(std::shared_ptr<VulkanRenderingContext> context,
                  std::string image_path,
                  int binding_point,
                  ShaderType shader_stage);
  void SetSampler(api::Sampler sampler) override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const override;
  ~VulkanTexture2D() override;
};
}

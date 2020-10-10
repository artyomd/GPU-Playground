//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/Texture2D.h"
#include "src/api/vulkan/VulkanRenderingContext.h"
#include "src/api/vulkan/VulkanUniform.h"

namespace api {
class VulkanTexture2D : public Texture2D, public VulkanUniform {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkDescriptorImageInfo image_info_{};
  VkSampler sampler_{};
  VkImageView image_view_{};
  VkImage image_{};
  VkDeviceMemory image_memory_{};

  void CreateTextureSampler();

 public:
  VulkanTexture2D(std::shared_ptr<VulkanRenderingContext> context,
                  std::string image_path,
                  int binding_point,
                  ShaderType shader_stage);
  ~VulkanTexture2D() override;
  [[nodiscard]] VkDescriptorSetLayoutBinding GetLayoutBinding() const override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const override;
};
}
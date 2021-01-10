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
  explicit VulkanTexture2D(std::shared_ptr<VulkanRenderingContext> context);
  void SetSampler(api::Sampler sampler) override;
  void Load(const std::string &path) override;
  void Load(size_t width, size_t height, const void *data) override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(unsigned int image_index,
                                                              unsigned int binding_point) const override;
  ~VulkanTexture2D();
};
}

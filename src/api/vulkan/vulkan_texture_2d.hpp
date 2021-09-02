//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/texture_2d.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanTexture2D : public Texture2D {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkDescriptorImageInfo image_info_{};
  VkImageView image_view_{};
  VkImage image_{};
  VkDeviceMemory image_memory_{};

 public:
  VulkanTexture2D(std::shared_ptr<VulkanRenderingContext> context,
                  uint32_t width,
                  uint32_t height,
                  PixelFormat pixel_format);
  void SetSampler(api::Sampler sampler) override;
  void Load(const void *data) override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(unsigned int image_index,
                                                              unsigned int binding_point) const;
  ~VulkanTexture2D();
};
}

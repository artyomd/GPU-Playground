//
// Created by artyomd on 3/30/20.
//

#pragma once

#include <api/Texture2D.h>
#include "VkUniform.h"
#include "VkRenderingContext.h"

namespace api {
class VkTexture2D : public Texture2D, public VkUniform {
 private:
  VkRenderingContext *context_;
  VkDevice *device_;
  VkDescriptorImageInfo image_info_{};
  VkSampler sampler_{};
  VkImageView image_view_{};
  VkImage image_{};
  VkDeviceMemory image_memory_{};

  void CreateTextureSampler();
  void CreateImage(uint32_t width,
                   uint32_t height,
                   VkFormat format,
                   VkImageTiling tiling,
                   VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkImage &image,
                   VkDeviceMemory &image_memory);
 public:
  VkTexture2D(VkRenderingContext *context, std::string image_path, int binding_point, ShaderType shader_stage);
  ~VkTexture2D() override;
  [[nodiscard]] VkDescriptorSetLayoutBinding GetLayoutBinding() const override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const override;
};
}
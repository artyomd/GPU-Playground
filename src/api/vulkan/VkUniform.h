//
// Created by artyomd on 3/30/20.
//
#pragma once
#include <api/Uniform.h>
#include <vulkan/vulkan.h>
namespace api {
class VkUniform {
 public:
  [[nodiscard]] virtual VkDescriptorSetLayoutBinding GetLayoutBinding() const = 0;
  [[nodiscard]] virtual VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const = 0;
};
}

//
// Created by artyomd on 3/30/20.
//
#pragma once

#include <vulkan/vulkan.h>

namespace api::vulkan {
class VulkanUniform {
 public:
  [[nodiscard]] virtual VkWriteDescriptorSet GetWriteDescriptorSetFor(unsigned int image_index,
                                                                      unsigned int binding_point) const = 0;
};
}

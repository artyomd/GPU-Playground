//
// Created by artyomd on 3/30/20.
//
#pragma once

#include <vulkan/vulkan.h>

#include "src/api/uniform.hpp"

namespace api::vulkan {
class VulkanUniform {
 public:
  [[nodiscard]] virtual VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const = 0;
};
}

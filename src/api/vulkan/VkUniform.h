//
// Created by artyomd on 3/30/20.
//
#pragma once
#include <api/Uniform.h>
#include <vulkan/vulkan.h>
namespace api {
class VkUniform : public Uniform {
 public:
  VkUniform(int binding_point, ShaderType shader_stage) : Uniform(binding_point, shader_stage) {}
  [[nodiscard]] virtual VkDescriptorSetLayoutBinding GetLayoutBinding() const = 0;
  [[nodiscard]] virtual VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const = 0;
};
}

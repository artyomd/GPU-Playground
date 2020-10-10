//
// Created by artyomd on 3/4/20.
//
#pragma once

#include <vulkan/vulkan.h>

#include "src/api/shader.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanShader : public Shader {
 private:
  VkDevice device_;
  VkShaderModule shader_module_ = nullptr;
  VkPipelineShaderStageCreateInfo shader_stage_info_{};
 public:
  VulkanShader(const std::shared_ptr<VulkanRenderingContext> &context,
               std::string sipr_v_shader_location,
               std::string glsl_shader_location,
               std::string entry_point_name,
               ShaderType type
  );

  [[nodiscard]] VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

  ~VulkanShader() override;
};
}
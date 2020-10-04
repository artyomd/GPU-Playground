//
// Created by artyomd on 3/4/20.
//
#pragma once

#include <vulkan/vulkan.h>

#include "src/api/Shader.h"
#include "src/api/vulkan/VkRenderingContext.h"

namespace api {
class VkShader : public Shader {
 private:
  VkShaderModule shader_module_;
  VkDevice *device_;
  VkPipelineShaderStageCreateInfo *shader_stage_info_;
 public:
  VkShader(VkRenderingContext *context,
           std::string
           sipr_v_shader_location,
           std::string glsl_shader_location,
           std::string
           entry_point_name,
           ShaderType type
  );

  [[nodiscard]] const VkPipelineShaderStageCreateInfo *GetShaderStageInfo() const;

  ~VkShader() override;
};
}
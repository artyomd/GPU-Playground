//
// Created by artyomd on 3/4/20.
//

#include "src/api/vulkan/VulkanShader.h"

#include "src/api/Utils.h"
#include "src/api/vulkan/VulkanUtils.h"

api::VulkanShader::VulkanShader(const std::shared_ptr<VulkanRenderingContext> &context,
                                std::string sipr_v_shader_location,
                                std::string glsl_shader_location,
                                std::string entry_point_name,
                                api::ShaderType type)
    : Shader(std::move(sipr_v_shader_location),
             std::move(glsl_shader_location),
             std::move(entry_point_name),
             type), device_(context->GetDevice()) {
  auto code = ReadFile(this->sipr_v_shader_location_);
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

  if (vkCreateShaderModule(device_, &create_info, nullptr, &shader_module_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  shader_stage_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_info_.stage = GetShaderVkType(type);
  shader_stage_info_.module = shader_module_;
  shader_stage_info_.pName = this->entry_point_name_.data();
}
api::VulkanShader::~VulkanShader() {
  vkDestroyShaderModule(device_, shader_module_, nullptr);
}
VkPipelineShaderStageCreateInfo api::VulkanShader::GetShaderStageInfo() const {
  return shader_stage_info_;
}

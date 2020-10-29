//
// Created by artyomd on 3/4/20.
//

#include "src/api/vulkan/vulkan_shader.hpp"

#include <cassert>
#include <vulkan/vulkan.h>
#include <spirv-reflect/spirv_reflect.h>

#include "src/api/utils.hpp"
#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanShader::VulkanShader(const std::shared_ptr<VulkanRenderingContext> &context,
                                        std::string sipr_v_shader_location,
                                        std::string entry_point_name,
                                        api::ShaderType type)
    : Shader(std::move(sipr_v_shader_location),
             std::move(entry_point_name),
             type),
      device_(context->GetDevice()) {
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

  SpvReflectShaderModule module = {};
  SpvReflectResult result = spvReflectCreateShaderModule(code.size(), code.data(), &module);
  assert(result == SPV_REFLECT_RESULT_SUCCESS);

  uint32_t count = 0;
  result = spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
  assert(result == SPV_REFLECT_RESULT_SUCCESS);

  std::vector<SpvReflectDescriptorSet *> sets(count);
  result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
  assert(result == SPV_REFLECT_RESULT_SUCCESS);

  // Demonstrates how to generate all necessary data structures to create a
  // VkDescriptorSetLayout for each descriptor set in this shader.
  for (size_t i_set = 0; i_set < sets.size(); ++i_set) {
    const SpvReflectDescriptorSet &refl_set = *(sets[i_set]);
    bindings_.resize(refl_set.binding_count);
    for (uint32_t i_binding = 0; i_binding < refl_set.binding_count; ++i_binding) {
      const SpvReflectDescriptorBinding &refl_binding = *(refl_set.bindings[i_binding]);
      VkDescriptorSetLayoutBinding &layout_binding = bindings_[i_binding];
      layout_binding.binding = refl_binding.binding;
      layout_binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);
      layout_binding.descriptorCount = 1;
      for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
        layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];
      }
      layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(module.shader_stage);
    }
  }

  spvReflectDestroyShaderModule(&module);
}

VkPipelineShaderStageCreateInfo api::vulkan::VulkanShader::GetShaderStageInfo() const {
  return shader_stage_info_;
}

std::vector<VkDescriptorSetLayoutBinding> api::vulkan::VulkanShader::GetBindings() const {
  return bindings_;
}

api::vulkan::VulkanShader::~VulkanShader() {
  vkDestroyShaderModule(device_, shader_module_, nullptr);
}

//
// Created by artyomd on 3/4/20.
//

#include "src/api/vulkan/vulkan_shader.hpp"

#include <snowhouse/snowhouse.h>

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

  SpvReflectResult result = spvReflectCreateShaderModule(code.size(), code.data(), &reflect_shader_module_);
  AssertThat(result, snowhouse::Is().EqualTo(SPV_REFLECT_RESULT_SUCCESS));

  uint32_t count = 0;
  result = spvReflectEnumerateDescriptorSets(&reflect_shader_module_, &count, nullptr);
  AssertThat(result, snowhouse::Is().EqualTo(SPV_REFLECT_RESULT_SUCCESS));
  if (count > 1) {
    throw std::runtime_error("unhandled");
  }

  std::vector<SpvReflectDescriptorSet *> sets(count);
  result = spvReflectEnumerateDescriptorSets(&reflect_shader_module_, &count, sets.data());
  AssertThat(result, snowhouse::Is().EqualTo(SPV_REFLECT_RESULT_SUCCESS));

  for (auto set:sets) {
    bindings_.resize(set->binding_count);
    for (uint32_t i_binding = 0; i_binding < set->binding_count; ++i_binding) {
      const SpvReflectDescriptorBinding &reflect_descriptor_binding = *(set->bindings[i_binding]);
      VkDescriptorSetLayoutBinding &layout_binding = bindings_[i_binding];
      layout_binding.binding = reflect_descriptor_binding.binding;
      layout_binding.descriptorType = static_cast<VkDescriptorType>(reflect_descriptor_binding.descriptor_type);
      layout_binding.descriptorCount = 1;
      for (uint32_t i_dim = 0; i_dim < reflect_descriptor_binding.array.dims_count; ++i_dim) {
        layout_binding.descriptorCount *= reflect_descriptor_binding.array.dims[i_dim];
      }
      layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(reflect_shader_module_.shader_stage);
    }
  }
}

VkPipelineShaderStageCreateInfo api::vulkan::VulkanShader::GetShaderStageInfo() const {
  return shader_stage_info_;
}

std::vector<VkDescriptorSetLayoutBinding> api::vulkan::VulkanShader::GetBindings() const {
  return bindings_;
}

size_t api::vulkan::VulkanShader::DescriptorSizeInBytes(unsigned int binding_point) const {
  uint32_t count = 1;
  std::vector<SpvReflectDescriptorSet *> sets(count);
  auto result = spvReflectEnumerateDescriptorSets(&reflect_shader_module_, &count, sets.data());
  AssertThat(result, snowhouse::Is().EqualTo(SPV_REFLECT_RESULT_SUCCESS));
  for (uint32_t i_binding = 0; i_binding < sets[0]->binding_count; ++i_binding) {
    const SpvReflectDescriptorBinding &reflect_descriptor_binding = *(sets[0]->bindings[i_binding]);
    if (reflect_descriptor_binding.binding != binding_point) {
      continue;
    }
    return reflect_descriptor_binding.block.size;
  }
  throw std::runtime_error("invalid binding point");
}

api::vulkan::VulkanShader::~VulkanShader() {
  spvReflectDestroyShaderModule(&reflect_shader_module_);
  vkDestroyShaderModule(device_, shader_module_, nullptr);
}

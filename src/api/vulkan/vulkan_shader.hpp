//
// Created by artyomd on 3/4/20.
//
#pragma once

#include <vulkan/vulkan.h>
#include <SPIRV-Reflect/spirv_reflect.h>

#include "src/api/shader.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"
#include "src/api/vulkan/vulkan_utils.hpp"

namespace api::vulkan {
class VulkanShader : public Shader {
 private:
  VkDevice device_;
  VkShaderModule shader_module_ = nullptr;
  SpvReflectShaderModule reflect_shader_module_{};
  VkPipelineShaderStageCreateInfo shader_stage_info_{};
  mutable VkSpecializationInfo shader_specialization_info_{};
  std::vector<VkSpecializationMapEntry> specialization_map_entries_{};
  void *spec_data_ = nullptr;
  size_t spec_data_size_ = 0;

  std::vector<VkDescriptorSetLayoutBinding> bindings_{};

 public:
  VulkanShader(const std::shared_ptr<VulkanRenderingContext> &context,
               std::string sipr_v_shader_location,
               std::string entry_point_name,
               ShaderType type
  );

  void SetConstant(unsigned int constant_id, bool constant_value) override;

  [[nodiscard]] VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

  [[nodiscard]] std::vector<VkDescriptorSetLayoutBinding> GetBindings() const;

  [[nodiscard]] size_t DescriptorSizeInBytes(unsigned int binding_point) const;

  ~VulkanShader() override;
};
}

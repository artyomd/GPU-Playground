#include "src/api/vulkan/vulkan_shader.hpp"

#include "src/api/vulkan/vulkan_utils.hpp"
#include "src/utils/check.hpp"
#include "src/utils/variant_utils.hpp"

api::vulkan::VulkanShader::VulkanShader(const std::shared_ptr<VulkanRenderingContext> &context,
                                        std::string sipr_v_shader_location,
                                        std::string entry_point_name,
                                        api::ShaderType type) : Shader(std::move(sipr_v_shader_location),
                                                                       std::move(entry_point_name),
                                                                       type), device_(context->GetDevice()) {
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code_.size();
  create_info.pCode = reinterpret_cast<const uint32_t *>(code_.data());

  if (vkCreateShaderModule(device_, &create_info, nullptr, &shader_module_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }
  shader_specialization_info_.dataSize = 0;
  shader_specialization_info_.pData = nullptr;
  shader_specialization_info_.pMapEntries = nullptr;
  shader_specialization_info_.mapEntryCount = 0;

  shader_stage_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_info_.stage = GetVkShaderStageFlag(type);
  shader_stage_info_.module = shader_module_;
  shader_stage_info_.pName = this->entry_point_name_.data();
  shader_stage_info_.pSpecializationInfo = &shader_specialization_info_;

  SpvReflectResult result = spvReflectCreateShaderModule(code_.size(), code_.data(), &reflect_shader_module_);

  uint32_t count = 0;
  result = spvReflectEnumerateEntryPointDescriptorSets(&reflect_shader_module_,
                                                       this->entry_point_name_.data(),
                                                       &count,
                                                       nullptr);
  CHECK(result == SPV_REFLECT_RESULT_SUCCESS, "spir-v reflection failed");
  if (count > 1) {
    throw std::runtime_error("unhandled");
  }

  std::vector<SpvReflectDescriptorSet *> sets(count);
  result = spvReflectEnumerateEntryPointDescriptorSets(&reflect_shader_module_,
                                                       this->entry_point_name_.data(),
                                                       &count,
                                                       sets.data());
  CHECK(result == SPV_REFLECT_RESULT_SUCCESS, "spir-v reflection failed");

  if (count == 1) {
    auto set = sets[0];
    bindings_.resize(set->binding_count);
    for (uint32_t i_binding = 0; i_binding < set->binding_count; ++i_binding) {
      const SpvReflectDescriptorBinding &reflect_descriptor_binding = *(set->bindings[i_binding]);
      VkDescriptorSetLayoutBinding &layout_binding = bindings_[i_binding];
      layout_binding.binding = reflect_descriptor_binding.binding;
      layout_binding.descriptorType = static_cast<VkDescriptorType>(
          reflect_descriptor_binding.descriptor_type);
      layout_binding.descriptorCount = 1;
      for (uint32_t i_dim = 0; i_dim < reflect_descriptor_binding.array.dims_count; ++i_dim) {
        layout_binding.descriptorCount *= reflect_descriptor_binding.array.dims[i_dim];
      }
      layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(
          reflect_shader_module_.shader_stage);
    }
  }
}

VkPipelineShaderStageCreateInfo api::vulkan::VulkanShader::GetShaderStageInfo() const {
  if (constants_changed_) {
    shader_specialization_info_.mapEntryCount = static_cast<uint32_t>(specs_.size());
    specialization_map_entries_.clear();
    spec_data_size_ = 0;
    for (const auto &entry: specs_) {
      VkSpecializationMapEntry specialization_map_entry{};
      specialization_map_entry.constantID = entry.first;
      specialization_map_entry.offset = static_cast<uint32_t>(spec_data_size_);
      VisitVariant(entry.second,
#define VISIT(data_type)                                                   \
  [&](data_type v) {                                                       \
    spec_data_ = realloc(spec_data_, spec_data_size_ + sizeof(data_type)); \
    *(reinterpret_cast<data_type *>(static_cast<char *>(spec_data_) +      \
                                    spec_data_size_)) = v;                 \
    spec_data_size_ += sizeof(data_type);                                  \
    specialization_map_entry.size = sizeof(data_type);                     \
  }
                   VISIT(bool), VISIT(unsigned int), VISIT(int), VISIT(float), VISIT(double));
      specialization_map_entries_.emplace_back(specialization_map_entry);
    }
    shader_specialization_info_.pMapEntries = specialization_map_entries_.data();
    shader_specialization_info_.dataSize = spec_data_size_;
    shader_specialization_info_.pData = spec_data_;
  }
  return shader_stage_info_;
}

std::vector<VkDescriptorSetLayoutBinding> api::vulkan::VulkanShader::GetBindings() const {
  return bindings_;
}

size_t api::vulkan::VulkanShader::DescriptorSizeInBytes(unsigned int binding_point) const {
  uint32_t count = 1;
  std::vector<SpvReflectDescriptorSet *> sets(count);
  auto result = spvReflectEnumerateDescriptorSets(&reflect_shader_module_, &count, sets.data());
  CHECK(result == SPV_REFLECT_RESULT_SUCCESS, "spir-v reflection failed");
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
  free(spec_data_);
}

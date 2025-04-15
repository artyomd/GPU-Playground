#include "shader.hpp"

#include <glm/ext/scalar_uint_sized.hpp>
#include <map>
#include <utility>

#include "utils.hpp"

std::shared_ptr<vulkan::Shader> vulkan::Shader::Create(const std::shared_ptr<RenderingContext> &context,
                                                       const std::vector<uint32_t> &spirv_code,
                                                       const std::string &entry_point_name) {
  return std::shared_ptr<Shader>(new Shader(context, spirv_code, entry_point_name));
}

vulkan::Shader::Shader(const std::shared_ptr<RenderingContext> &context, const std::vector<uint32_t> &spirv_code,
                       std::string entry_point_name)
    : device_(context->GetDevice()), spirv_code_(spirv_code), entry_point_name_(std::move(entry_point_name)) {
  const VkShaderModuleCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = spirv_code.size() * sizeof(uint32_t),
      .pCode = spirv_code_.data(),
  };
  VK_CALL(vkCreateShaderModule(device_, &create_info, nullptr, &shader_module_));

  SpvReflectResult result =
      spvReflectCreateShaderModule(spirv_code_.size() * sizeof(uint32_t), spirv_code_.data(), &reflect_shader_module_);
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  switch (reflect_shader_module_.shader_stage) {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
      this->type_ = VK_SHADER_STAGE_VERTEX_BIT;
      break;
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
      this->type_ = VK_SHADER_STAGE_FRAGMENT_BIT;
      break;
    case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
      this->type_ = VK_SHADER_STAGE_COMPUTE_BIT;
      break;
    default:
      throw std::runtime_error("unhandled shader stage");
  }

  uint32_t count = 0;
  result = spvReflectEnumerateEntryPointDescriptorSets(&reflect_shader_module_, this->entry_point_name_.data(), &count,
                                                       nullptr);
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  if (count > 1) {
    throw std::runtime_error("unhandled");
  }
  std::vector<SpvReflectDescriptorSet *> sets(count);
  result = spvReflectEnumerateEntryPointDescriptorSets(&reflect_shader_module_, this->entry_point_name_.data(), &count,
                                                       sets.data());
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  if (count == 1) {
    const auto *set = sets[0];
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

VkPipelineShaderStageCreateInfo vulkan::Shader::GetShaderStageInfo() const {
  if (constants_changed_) {
    spec_map_entries_.clear();
    uint32_t data_offset = 0;
    spec_data_.clear();
    for (const auto &[id, value] : specs_) {
      VkSpecializationMapEntry specialization_map_entry{
          .constantID = id,
          .offset = data_offset,
      };
      std::visit(
          [this, &data_offset, &specialization_map_entry](auto value) {
            spec_data_.resize(spec_data_.size() + sizeof(value));
            memcpy(&spec_data_[data_offset], &value, sizeof(value));
            data_offset += sizeof(value);
            specialization_map_entry.size = sizeof(value);
          },
          value);
      spec_map_entries_.emplace_back(specialization_map_entry);
    }
    shader_spec_info_ = {
        .mapEntryCount = static_cast<uint32_t>(specs_.size()),
        .pMapEntries = spec_map_entries_.data(),
        .dataSize = spec_data_.size(),
        .pData = spec_data_.data(),
    };
  }

  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = type_,
      .module = shader_module_,
      .pName = this->entry_point_name_.data(),
      .pSpecializationInfo = &shader_spec_info_,
  };
}

std::vector<VkDescriptorSetLayoutBinding> vulkan::Shader::GetBindings() const { return bindings_; }

uint32_t vulkan::Shader::DescriptorSizeInBytes(const uint32_t binding_point) const {
  uint32_t count = 1;
  std::vector<SpvReflectDescriptorSet *> sets(count);
  if (const auto result = spvReflectEnumerateDescriptorSets(&reflect_shader_module_, &count, sets.data());
      result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  for (uint32_t i_binding = 0; i_binding < sets[0]->binding_count; ++i_binding) {
    const SpvReflectDescriptorBinding &reflect_descriptor_binding = *(sets[0]->bindings[i_binding]);
    if (reflect_descriptor_binding.binding != binding_point) {
      continue;
    }
    return reflect_descriptor_binding.block.size;
  }
  throw std::runtime_error("invalid binding point");
}

vulkan::Shader::~Shader() {
  spvReflectDestroyShaderModule(&reflect_shader_module_);
  vkDestroyShaderModule(device_, shader_module_, nullptr);
}
template <>
void vulkan::Shader::SetConstant(const unsigned int constant_id, const bool constant_value) {
  constants_changed_ = true;
  specs_.insert_or_assign(constant_id, static_cast<VkBool32>(constant_value));
}

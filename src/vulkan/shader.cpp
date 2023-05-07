#include "shader.hpp"

#include "utils.hpp"

#include <shaderc/shaderc.hpp>

#include <fstream>
#include <map>
#include <sstream>

namespace {
std::map<uint32_t, shaderc_spirv_version> vk_spir_version_mapping = {
    {VK_API_VERSION_1_0, shaderc_spirv_version_1_0},
    {VK_API_VERSION_1_1, shaderc_spirv_version_1_3},
    {VK_API_VERSION_1_2, shaderc_spirv_version_1_5},
    {VK_API_VERSION_1_3, shaderc_spirv_version_1_6},
};
} // namespace

std::shared_ptr<vulkan::Shader> vulkan::Shader::Create(const std::shared_ptr<RenderingContext> &context,
                                                       const std::string &glsl_file_path,
                                                       const std::string &entry_point_name,
                                                       VkShaderStageFlagBits type) {
  return std::shared_ptr<Shader>(new vulkan::Shader(context, glsl_file_path, entry_point_name, type));
}

vulkan::Shader::Shader(const std::shared_ptr<RenderingContext> &context,
                       const std::string &file_path,
                       const std::string &entry_point_name,
                       VkShaderStageFlagBits type) : device_(context->GetDevice()),
                                                     entry_point_name_(entry_point_name),
                                                     type_(type) {
  std::ifstream file_stream(file_path, std::ios::in);
  std::string content;
  if (!file_stream.is_open()) {
    throw std::runtime_error("invalid file path:" + file_path);
  } else {
    std::ostringstream string_stream;
    string_stream << file_stream.rdbuf();
    content = string_stream.str();
  }
  if (content.empty()) {
    throw std::runtime_error("file is empty or unable to read file, path:" + file_path);
  }

  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  {
    options.SetSourceLanguage(shaderc_source_language_glsl);
    options.SetForcedVersionProfile(460, shaderc_profile_none);
    options.SetInvertY(false);
    options.SetTargetEnvironment(shaderc_target_env_vulkan, context->GetPhysicalDeviceVkSpecVersion());
    options.SetWarningsAsErrors();
    options.SetTargetSpirv(vk_spir_version_mapping[context->GetPhysicalDeviceVkSpecVersion()]);
#if defined(NDEBUG)
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
#else
    options.SetOptimizationLevel(shaderc_optimization_level_zero);
    options.SetGenerateDebugInfo();
#endif
  }
  auto get_shader_stage = [](VkShaderStageFlagBits type) {
    switch (type) {
      case VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT:return shaderc_shader_kind::shaderc_glsl_fragment_shader;
      case VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT:return shaderc_shader_kind::shaderc_glsl_vertex_shader;
      default:throw std::runtime_error("unhandled");
    }
  };
  compiled_spirv_ = compiler.CompileGlslToSpv(content,
                                              get_shader_stage(type),
                                              file_path.data(),
                                              entry_point_name.data(),
                                              options);
  if (compiled_spirv_.GetCompilationStatus() != shaderc_compilation_status_success) {
    throw std::runtime_error("shader compilation failed, shader path:" + file_path + compiled_spirv_.GetErrorMessage());
  }
  std::vector<uint32_t> spirv;
  spirv.assign(compiled_spirv_.cbegin(), compiled_spirv_.cend());

  VkShaderModuleCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = spirv.size() * sizeof(uint32_t),
      .pCode = spirv.data(),
  };
  VK_CALL(vkCreateShaderModule(device_, &create_info, nullptr, &shader_module_));

  SpvReflectResult
      result = spvReflectCreateShaderModule(spirv.size() * sizeof(uint32_t), spirv.data(), &reflect_shader_module_);
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }

  uint32_t count = 0;
  result = spvReflectEnumerateEntryPointDescriptorSets(&reflect_shader_module_,
                                                       this->entry_point_name_.data(),
                                                       &count,
                                                       nullptr);
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  if (count > 1) {
    throw std::runtime_error("unhandled");
  }
  std::vector<SpvReflectDescriptorSet *> sets(count);
  result = spvReflectEnumerateEntryPointDescriptorSets(&reflect_shader_module_,
                                                       this->entry_point_name_.data(),
                                                       &count,
                                                       sets.data());
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  if (count == 1) {
    auto *set = sets[0];
    bindings_.resize(set->binding_count);
    for (uint32_t i_binding = 0; i_binding < set->binding_count; ++i_binding) {
      const SpvReflectDescriptorBinding &kReflectDescriptorBinding = *(set->bindings[i_binding]);
      VkDescriptorSetLayoutBinding &layout_binding = bindings_[i_binding];
      layout_binding.binding = kReflectDescriptorBinding.binding;
      layout_binding.descriptorType = static_cast<VkDescriptorType>(kReflectDescriptorBinding.descriptor_type);
      layout_binding.descriptorCount = 1;
      for (uint32_t i_dim = 0; i_dim < kReflectDescriptorBinding.array.dims_count; ++i_dim) {
        layout_binding.descriptorCount *= kReflectDescriptorBinding.array.dims[i_dim];
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
    for (const auto &kEntry : specs_) {
      VkSpecializationMapEntry specialization_map_entry{
          .constantID = kEntry.first,
          .offset = data_offset,
      };
      std::visit([this, &data_offset, &specialization_map_entry](auto value) {
        spec_data_.resize(spec_data_.size() + sizeof(value));
        memcpy(&spec_data_[data_offset], &value, sizeof(value));
        data_offset += sizeof(value);
        specialization_map_entry.size = sizeof(value);
      }, kEntry.second);
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

std::vector<VkDescriptorSetLayoutBinding> vulkan::Shader::GetBindings() const {
  return bindings_;
}

size_t vulkan::Shader::DescriptorSizeInBytes(unsigned int binding_point) const {
  uint32_t count = 1;
  std::vector<SpvReflectDescriptorSet *> sets(count);
  auto result = spvReflectEnumerateDescriptorSets(&reflect_shader_module_, &count, sets.data());
  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    throw std::runtime_error("spir-v reflection failed");
  }
  for (uint32_t i_binding = 0; i_binding < sets[0]->binding_count; ++i_binding) {
    const SpvReflectDescriptorBinding &kReflectDescriptorBinding = *(sets[0]->bindings[i_binding]);
    if (kReflectDescriptorBinding.binding != binding_point) {
      continue;
    }
    return kReflectDescriptorBinding.block.size;
  }
  throw std::runtime_error("invalid binding point");
}

vulkan::Shader::~Shader() {
  spvReflectDestroyShaderModule(&reflect_shader_module_);
  vkDestroyShaderModule(device_, shader_module_, nullptr);
}
template<>
void vulkan::Shader::SetConstant(unsigned int constant_id, bool constant_value) {
  constants_changed_ = true;
  specs_.insert_or_assign(constant_id, static_cast<VkBool32>(constant_value));
}

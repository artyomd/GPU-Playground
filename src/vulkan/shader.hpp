#pragma once

#include "rendering_context.hpp"

#include <spirv_reflect.h>

#include <map>
#include <memory>
#include <variant>

namespace vulkan {
class Shader {
 public:
  static std::shared_ptr<Shader> Create(const std::shared_ptr<RenderingContext> &context,
                                        const std::vector<uint32_t> &spirv_code,
                                        const std::string &entry_point_name);

  Shader() = delete;
  Shader(const Shader &) = delete;
  [[nodiscard]] VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

  [[nodiscard]] std::vector<VkDescriptorSetLayoutBinding> GetBindings() const;

  [[nodiscard]] size_t DescriptorSizeInBytes(unsigned int binding_point) const;

  template<typename T>
  void SetConstant(unsigned int constant_id, T constant_value) {
    static_assert(std::is_same<T, int>::value or
                      std::is_same<T, unsigned int>::value or
                      std::is_same<T, float>::value or
                      std::is_same<T, double>::value,
                  "type is not supported by spir-v spec");
    constants_changed_ = true;
    specs_.insert_or_assign(constant_id, constant_value);
  }

  ~Shader();
 private:
  Shader(const std::shared_ptr<RenderingContext> &context,
         const std::vector<uint32_t> &spirv_code,
         const std::string &entry_point_name);

  VkDevice device_ = VK_NULL_HANDLE;
  VkShaderModule shader_module_ = VK_NULL_HANDLE;
  SpvReflectShaderModule reflect_shader_module_{};

  std::vector<uint32_t> spirv_code_{};
  std::string entry_point_name_;
  VkShaderStageFlagBits type_;

  std::vector<VkDescriptorSetLayoutBinding> bindings_;

  bool constants_changed_ = false;
  std::map<unsigned int, std::variant<int, unsigned int, float, double>> specs_{};

  mutable VkSpecializationInfo shader_spec_info_{};
  mutable std::vector<VkSpecializationMapEntry> spec_map_entries_{};
  mutable std::vector<std::byte> spec_data_;
};
template<>
void Shader::SetConstant(unsigned int constant_id, bool constant_value);
}  // namespace vulkan

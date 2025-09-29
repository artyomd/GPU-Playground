#pragma once

#include <spirv_reflect.h>

#include <map>
#include <memory>
#include <variant>

#include "rendering_context.hpp"

namespace vulkan {
class Shader final {
 public:
  static std::shared_ptr<Shader> Create(const std::shared_ptr<RenderingContext> &context,
                                        const std::vector<uint32_t> &spirv_code, const std::string &entry_point_name);

  Shader() = delete;
  Shader(const Shader &) = delete;
  Shader(Shader &&) = delete;

  Shader &operator=(const Shader &) = delete;
  Shader &operator=(Shader &&) = delete;

  [[nodiscard]] VkPipelineShaderStageCreateInfo GetShaderStageInfo() const;

  [[nodiscard]] std::map<uint32_t, SpvReflectDescriptorBinding> GetBindings() const;

  template <typename T>
  void SetConstant(const uint32_t constant_id, const T constant_value) {
    static_assert(std::is_same_v<T, int> or std::is_same_v<T, unsigned int> or std::is_same_v<T, float> or
                      std::is_same_v<T, double>,
                  "type is not supported by spir-v spec");
    constants_changed_ = true;
    specs_.insert_or_assign(constant_id, constant_value);
  }

  ~Shader();

 private:
  Shader(const std::shared_ptr<RenderingContext> &context, const std::vector<uint32_t> &spirv_code,
         std::string entry_point_name);

  VkDevice device_ = VK_NULL_HANDLE;
  VkShaderModule shader_module_ = VK_NULL_HANDLE;
  SpvReflectShaderModule reflect_shader_module_{};

  std::vector<uint32_t> spirv_code_;
  std::string entry_point_name_;
  VkShaderStageFlagBits type_;

  std::map<uint32_t, SpvReflectDescriptorBinding> bindings_;

  bool constants_changed_ = false;
  std::map<uint32_t, std::variant<int, unsigned int, float, double>> specs_;

  mutable VkSpecializationInfo shader_spec_info_{};
  mutable std::vector<VkSpecializationMapEntry> spec_map_entries_;
  mutable std::vector<std::byte> spec_data_;
};

template <>
void Shader::SetConstant(uint32_t constant_id, bool constant_value);
}  // namespace vulkan

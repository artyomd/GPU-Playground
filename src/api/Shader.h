#pragma once

#include <utility>
#include <string>
#include <utility>
#include <vulkan/vulkan.h>

namespace api {
enum ShaderType {
  SHADER_TYPE_VERTEX,
  SHADER_TYPE_FRAGMENT
};

VkShaderStageFlags GetVkShaderStageFlag(api::ShaderType shader_type);

int GetShaderGlType(ShaderType shader_type);

VkShaderStageFlagBits GetShaderVkType(ShaderType shader_type);

class Shader {

 protected:
  const std::string sipr_v_shader_location_;
  const std::string glsl_shader_location_;
  const std::string entry_point_name_;
  ShaderType type_;
 public:
  Shader(std::string sipr_v_shader_location,
         std::string glsl_shader_location,
         std::string entry_point_name,
         ShaderType type)
      : sipr_v_shader_location_(std::move(sipr_v_shader_location)),
        glsl_shader_location_(std::move(glsl_shader_location)),
        entry_point_name_(std::move(entry_point_name)),
        type_(type) {}

  virtual ~Shader() = default;
};
}
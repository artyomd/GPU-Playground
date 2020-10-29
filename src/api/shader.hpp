#pragma once

#include <string>
#include <utility>

namespace api {
enum class ShaderType {
  SHADER_TYPE_VERTEX,
  SHADER_TYPE_FRAGMENT,
  COUNT,
};

class Shader {
 protected:
  const std::string sipr_v_shader_location_;
  const std::string entry_point_name_;
  ShaderType type_;

  Shader(std::string sipr_v_shader_location,
         std::string entry_point_name,
         ShaderType type)
      : sipr_v_shader_location_(std::move(sipr_v_shader_location)),
        entry_point_name_(std::move(entry_point_name)),
        type_(type) {}

 public:
  virtual ~Shader() = default;
};
}

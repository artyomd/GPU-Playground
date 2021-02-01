#pragma once

#include <string>
#include <utility>
#include <map>
#include <memory>
#include <variant>

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
  bool constants_changed_ = false;
  std::map<unsigned int, std::variant<bool, int, unsigned int, float, double>> specs_{};

  Shader(std::string sipr_v_shader_location,
         std::string entry_point_name,
         ShaderType type)
      : sipr_v_shader_location_(std::move(sipr_v_shader_location)),
        entry_point_name_(std::move(entry_point_name)),
        type_(type) {}

 public:
  void SetConstant(unsigned int constant_id, bool constant_value) {
    constants_changed_ = true;
    specs_.insert_or_assign(constant_id, constant_value);
  }

  virtual ~Shader() = default;
};
}

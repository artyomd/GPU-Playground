#pragma once

#include <string>
#include <utility>
#include <map>
#include <memory>

namespace api {
enum class ShaderType {
  SHADER_TYPE_VERTEX,
  SHADER_TYPE_FRAGMENT,
  COUNT,
};

class SpecEntryBase {
 public:
  virtual void *GetData() = 0;
  virtual size_t GetDataSize() = 0;
};

template<typename T>
struct SpecEntry : public SpecEntryBase {
  T value{};
  void *GetData() override {
    return &value;
  }
  size_t GetDataSize() override {
    return sizeof(T);
  }
};

class Shader {
 protected:
  const std::string sipr_v_shader_location_;
  const std::string entry_point_name_;
  ShaderType type_;
  bool constants_changed_ = false;
  std::map<unsigned int, std::shared_ptr<SpecEntryBase>> specs_{};

  bool constants_changed_ = false;
  std::map<GLuint, std::shared_ptr<SpecEntryBase>> specs_{};

  Shader(std::string sipr_v_shader_location,
         std::string entry_point_name,
         ShaderType type)
      : sipr_v_shader_location_(std::move(sipr_v_shader_location)),
        entry_point_name_(std::move(entry_point_name)),
        type_(type) {}

 public:
  void SetConstant(unsigned int constant_id, bool constant_value) {
    constants_changed_ = true;
    SpecEntry<bool> entry{};
    entry.value = constant_value;
    specs_.insert_or_assign(constant_id, std::make_shared<SpecEntry<bool>>(entry));
  }

  virtual ~Shader() = default;
};
}

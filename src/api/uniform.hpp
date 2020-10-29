//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/shader.hpp"

namespace api {
class Uniform {
 protected:
  int binding_point_;
  api::ShaderType shader_stage_;

  Uniform(int binding_point, ShaderType shader_stage) : binding_point_(binding_point), shader_stage_(shader_stage) {}

 public:
  [[nodiscard]] int GetBindingPoint() const {
    return binding_point_;
  }

  [[nodiscard]] ShaderType GetShaderStage() const {
    return shader_stage_;
  }
  virtual ~Uniform() = default;
};
}

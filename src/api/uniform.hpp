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
 public:
  Uniform(int binding_point, ShaderType shader_stage) : binding_point_(binding_point), shader_stage_(shader_stage) {}
  virtual ~Uniform() = default;
};
}
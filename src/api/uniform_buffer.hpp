//
// Created by artyomd on 3/31/20.
//

#pragma once

#include "src/api/uniform.hpp"

namespace api {
class UniformBuffer : public Uniform {
 protected:
  UniformBuffer(int binding_point, ShaderType shader_stage) : Uniform(binding_point, shader_stage) {}
 public:
  virtual void Update(const void *data) = 0;
};
}

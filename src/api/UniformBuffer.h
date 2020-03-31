//
// Created by artyomd on 3/31/20.
//

#pragma once

#include "Uniform.h"
namespace api {
class UniformBuffer : public Uniform {
 public:
  UniformBuffer(int binding_point, ShaderType shader_stage) : Uniform(binding_point, shader_stage) {}
  virtual void Update(const void *data) = 0;
};
}
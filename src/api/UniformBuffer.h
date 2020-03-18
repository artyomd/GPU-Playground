//
// Created by artyomd on 3/7/20.
//
#pragma once

#include <Shader.h>

namespace api {
class UniformBuffer {
 protected:
  const int length_;
  int binding_location_;
  ShaderType shader_stage_;
 public:
  UniformBuffer(int length, int binding_point, ShaderType shader_stage) :
      binding_location_(binding_point), shader_stage_(shader_stage), length_(length) {};

  virtual void Update(const void *data) = 0;

  virtual void Bind() const = 0;

  virtual ~UniformBuffer() = default;
};
}
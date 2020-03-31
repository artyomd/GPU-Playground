//
// Created by artyomd on 3/10/20.
//
#pragma once

#include <api/Shader.h>
#include <GL/gl.h>
#include <api/UniformBuffer.h>
#include "GlUniform.h"
namespace api {
class GlUniformBuffer : public UniformBuffer, public GlUniform {
 private:
  int length_;
 public:
  GlUniformBuffer(int length, int binding_point, api::ShaderType shader_stage);
  void Bind() const override;
  void Unbind() const override;
  void Update(const void *data) override;
  ~GlUniformBuffer() override;
};
}
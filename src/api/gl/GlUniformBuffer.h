//
// Created by artyomd on 3/10/20.
//
#pragma once

#include "src/api/Shader.h"
#include "src/api/gl/GlUniform.h"
#include "src/api/UniformBuffer.h"

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
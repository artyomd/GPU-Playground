//
// Created by artyomd on 3/10/20.
//
#pragma once

#include "src/api/shader.hpp"
#include "src/api/opengl/opengl_uniform.hpp"
#include "src/api/uniform_buffer.hpp"

namespace api::opengl {
class OpenGlUniformBuffer : public UniformBuffer, public OpenGlUniform {
 private:
  size_t size_in_bytes_;
 public:
  OpenGlUniformBuffer(size_t size_in_bytes, int binding_point, api::ShaderType shader_stage);
  void Bind() const override;
  void Unbind() const override;
  void Update(const void *data) override;
  ~OpenGlUniformBuffer() override;
};
}

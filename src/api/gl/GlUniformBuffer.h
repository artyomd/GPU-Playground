//
// Created by artyomd on 3/10/20.
//
#pragma once
#include <api/UniformBuffer.h>
#include <api/Shader.h>
#include <GL/gl.h>
namespace api {
class GlUniformBuffer : public UniformBuffer {
 private:
  GLuint render_id_ = -1;
 public:
  GlUniformBuffer(int length, int binding_point, api::ShaderType shader_stage);
  void Bind() const override;
  void Unbind() const;
  void Update(const void *data) override;
  ~GlUniformBuffer() override;
};
}
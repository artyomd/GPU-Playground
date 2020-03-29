//
// Created by artyomd on 3/30/20.
//

#pragma once

#include <api/Uniform.h>
#include <GL/gl.h>

namespace api {
class GlUniform : public Uniform {
 protected:
  GLuint render_id_ = -1;
 public:
  GlUniform(int binding_point, ShaderType shader_stage) : Uniform(binding_point, shader_stage) {}
  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
};
}
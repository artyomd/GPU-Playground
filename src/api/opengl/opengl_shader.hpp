//
// Created by artyomd on 1/5/20.
//
#pragma once

#include <GL/glew.h>

#include "src/api/shader.hpp"

namespace api::opengl {
class OpenGlShader : public Shader {
 private:
  GLuint renderer_id_;
 public:
  OpenGlShader(std::string sipr_v_shader_location,
               std::string glsl_shader_location,
               std::string entry_point_name,
               ShaderType type);

  void AttachShader(GLuint program_id) const;

  void DetachShader(GLuint program_id) const;

  ~OpenGlShader() override;
};
}
//
// Created by artyomd on 1/5/20.
//
#pragma once

#include <GL/glew.h>

#include "src/api/shader.hpp"

namespace api::opengl {
class OpenGlShader : public Shader {
 private:
  GLuint shader_id_ = 0;
 public:
  OpenGlShader(std::string sipr_v_shader_location,
               std::string entry_point_name,
               ShaderType type);

  [[nodiscard]] GLuint GetShaderId() const;

  void SetConstant(unsigned int constant_id, bool constant_value) override;

  ~OpenGlShader() override;
};
}

#pragma once

#include <GL/glew.h>

#include "src/api/shader.hpp"

namespace api::opengl {
class OpenGlShader : public Shader {
 private:
  GLuint shader_id_ = 0;
  mutable bool specialized_ = false;
  mutable void *spec_data_ = nullptr;
  mutable size_t spec_data_size_ = 0;
 public:
  OpenGlShader(std::string sipr_v_shader_location, std::string entry_point_name, ShaderType type);

  [[nodiscard]] GLuint GetShaderId() const;

  ~OpenGlShader() override;
};
}

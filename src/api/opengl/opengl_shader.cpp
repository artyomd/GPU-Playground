//
// Created by artyomd on 1/5/20.
//

#include "src/api/opengl/opengl_shader.hpp"

#include <iostream>

#include "src/api/opengl/opengl_utils.hpp"
#include "src/api/utils.hpp"

api::opengl::OpenGlShader::OpenGlShader(std::string sipr_v_shader_location,
                                        std::string entry_point_name,
                                        api::ShaderType type) :
    Shader(std::move(sipr_v_shader_location),
           std::move(entry_point_name),
           type) {
  GL_CALL(shader_id_ = glCreateShader(GetShaderGlType(type)));
  AssertThat(shader_id_, snowhouse::Is().Not().EqualTo(0));

  auto code = ReadFile(this->sipr_v_shader_location_);
  GL_CALL(glShaderBinary(1, &shader_id_, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), code.size()));

  GL_CALL(glSpecializeShader(shader_id_, static_cast<const GLchar *>(entry_point_name_.c_str()), 0, nullptr, nullptr));

  GLint result;
  GL_CALL(glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &result));

  if (result == GL_FALSE) {
    GLint length;
    GL_CALL(glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &length));
    char *message = static_cast<char *>(alloca(static_cast<unsigned long>(length) * sizeof(char)));
    GL_CALL(glGetShaderInfoLog(shader_id_, length, &length, message));
    std::cout << "Failed to compile shader" << std::endl;
    GL_CALL(glDeleteShader(shader_id_));
    throw std::runtime_error(message);
  }

}

GLuint api::opengl::OpenGlShader::GetShaderId() const {
  return shader_id_;
}

api::opengl::OpenGlShader::~OpenGlShader() {
  GL_CALL(glDeleteShader(shader_id_));
}

//
// Created by artyomd on 1/5/20.
//

#include "src/api/opengl/opengl_shader.hpp"

#include <iostream>

#include "src/api/opengl/opengl_utils.hpp"
#include "src/api/utils.hpp"

api::opengl::OpenGlShader::OpenGlShader(std::string sipr_v_shader_location,
                                        std::string glsl_shader_location,
                                        std::string entry_point_name,
                                        api::ShaderType type) :
    Shader(std::move(sipr_v_shader_location),
           std::move(glsl_shader_location),
           std::move(entry_point_name),
           type) {
  GL_CALL(renderer_id_ = glCreateShader(GetShaderGlType(type)));
  assert(renderer_id_ != 0);

  auto data = ParseFile(this->glsl_shader_location_);
  const char *src = data.c_str();
  GL_CALL(glShaderSource(renderer_id_, 1, &src, nullptr));
  GL_CALL(glCompileShader(renderer_id_));

  GLint result;
  GL_CALL(glGetShaderiv(renderer_id_, GL_COMPILE_STATUS, &result));

  if (result == GL_FALSE) {
    GLint length;
    GL_CALL(glGetShaderiv(renderer_id_, GL_INFO_LOG_LENGTH, &length));
    char *message = (char *) alloca(length * sizeof(char));
    GL_CALL(glGetShaderInfoLog(renderer_id_, length, &length, message));
    std::cout << "Failed to compile shader" << std::endl;
    GL_CALL(glDeleteShader(renderer_id_));
    throw std::runtime_error(message);
  }

}

void api::opengl::OpenGlShader::AttachShader(GLuint program_id) const {
  GL_CALL(glAttachShader(program_id, renderer_id_));
}

void api::opengl::OpenGlShader::DetachShader(GLuint program_id) const {
  GL_CALL(glDetachShader(program_id, renderer_id_));
}

api::opengl::OpenGlShader::~OpenGlShader() {
  GL_CALL(glDeleteShader(renderer_id_));
}
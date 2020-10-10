//
// Created by artyomd on 1/6/20.
//

#include "src/api/opengl/opengl_utils.hpp"

#include <iostream>

void api::opengl::GlClearError() {
  while (glGetError() != GL_NO_ERROR);
}

bool api::opengl::GlLogCall(const char *function, const char *file, int line) {
  if (GLenum error = glGetError()) {
    std::cout << "[OpenGL error] (" << error << "): "
              << function << ""
              << file << ":"
              << line << std::endl;
    return false;
  }
  return true;
}

GLint api::opengl::GetGlType(api::DataType type) {
  switch (type) {
    case api::DataType::DATA_TYPE_BYTE:return GL_UNSIGNED_BYTE;
    case api::DataType::DATA_TYPE_UINT_16:return GL_UNSIGNED_SHORT;
    case api::DataType::DATA_TYPE_UINT_32:return GL_UNSIGNED_INT;
    case api::DataType::DATA_TYPE_FLOAT:return GL_FLOAT;
    default:throw std::runtime_error("unsupported enum");
  }
}

int api::opengl::GetShaderGlType(api::ShaderType shader_type) {
  switch (shader_type) {
    case api::ShaderType::SHADER_TYPE_VERTEX:return GL_VERTEX_SHADER;
    case api::ShaderType::SHADER_TYPE_FRAGMENT:return GL_FRAGMENT_SHADER;
    default:throw std::runtime_error("invalid shader type");
  }
}

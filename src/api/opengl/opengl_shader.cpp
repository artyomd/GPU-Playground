//
// Created by artyomd on 1/5/20.
//

#include "src/api/opengl/opengl_shader.hpp"

#include <iostream>

#include "src/api/opengl/opengl_utils.hpp"
#include "src/utils/variant_utils.hpp"

api::opengl::OpenGlShader::OpenGlShader(std::string sipr_v_shader_location,
                                        std::string entry_point_name,
                                        api::ShaderType type) :
    Shader(std::move(sipr_v_shader_location),
           std::move(entry_point_name),
           type) {
  GL_CALL(shader_id_ = glCreateShader(GetShaderGlType(type)));
  AssertThat(shader_id_, snowhouse::Is().Not().EqualTo(0));
}

GLuint api::opengl::OpenGlShader::GetShaderId() const {
  if (!specialized_ || constants_changed_) {
    GL_CALL(glShaderBinary(1, &shader_id_, GL_SHADER_BINARY_FORMAT_SPIR_V, code_.data(), code_.size()));
    GLuint indices[specs_.size()];
    int i = 0;
    spec_data_size_ = 0;
    for (const auto &entry : specs_) {
      indices[i] = entry.first;
      i++;
      auto value = entry.second;
      VisitVariant(value,
#define VISIT(data_type) \
  [&](data_type v) {\
  spec_data_ = realloc(spec_data_, spec_data_size_ + sizeof(GLuint));\
  *(reinterpret_cast< GLuint *>(static_cast<char*>(spec_data_)+spec_data_size_)) = v; \
  spec_data_size_+= sizeof(GLuint);\
  }
                   VISIT(bool),
                   VISIT(unsigned int),
                   VISIT(int),
                   VISIT(float),
                   VISIT(double)
      );
    }
    GL_CALL(glSpecializeShader(shader_id_,
                               static_cast<const GLchar *>(entry_point_name_.c_str()),
                               specs_.size(),
                               &indices[0],
                               reinterpret_cast<GLuint *>(spec_data_)));

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
    specialized_ = true;
  }
  return shader_id_;
}

api::opengl::OpenGlShader::~OpenGlShader() {
  GL_CALL(glDeleteShader(shader_id_));
  free(spec_data_);
}

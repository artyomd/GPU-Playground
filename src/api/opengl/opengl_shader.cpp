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
}

GLuint api::opengl::OpenGlShader::GetShaderId() const {
  if (!specialized_) {
    auto code = ReadFile(this->sipr_v_shader_location_);
    GL_CALL(glShaderBinary(1, &shader_id_, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), code.size()));
    GLuint indices[specs_.size()];
    int i = 0;
    void *data = nullptr;
    size_t data_size = 0;
    for (const auto &entry : specs_) {
      indices[9] = entry.first;
      auto value = entry.second;
      data_size += value->GetDataSize();
      data = realloc(data, data_size);
      memcpy(static_cast<char *>(data) + (data_size - value->GetDataSize(), value->GetData(), value->GetDataSize());
    }
    GL_CALL(glSpecializeShader(shader_id_,
                               static_cast<const GLchar *>(entry_point_name_.c_str()),
                               specs_.size(),
                               &indices[0],
                               reinterpret_cast<GLuint *>(data)));

    GLint result;
    GL_CALL(glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &result));
    free(data);
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
  return shader_id_;
}

void api::opengl::OpenGlShader::SetConstant(unsigned int constant_id, bool constant_value) {

}

api::opengl::OpenGlShader::~OpenGlShader() {
  GL_CALL(glDeleteShader(shader_id_));
}

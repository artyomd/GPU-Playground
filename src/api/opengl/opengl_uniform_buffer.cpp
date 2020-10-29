//
// Created by artyomd on 3/10/20.
//

#include "src/api/opengl/opengl_uniform_buffer.hpp"

#include <GL/glew.h>

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlUniformBuffer::OpenGlUniformBuffer(size_t size_in_bytes,
                                                      int binding_point,
                                                      api::ShaderType shader_stage)
    : UniformBuffer(binding_point, shader_stage),
      size_in_bytes_(size_in_bytes) {
  GL_CALL(glGenBuffers(1, &uniform_id_));
}

void api::opengl::OpenGlUniformBuffer::Update(const void *data) {
  GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, uniform_id_));
  GL_CALL(glBufferData(GL_UNIFORM_BUFFER, size_in_bytes_, data, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
void api::opengl::OpenGlUniformBuffer::Bind() const {
  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_, uniform_id_));
}
void api::opengl::OpenGlUniformBuffer::Unbind() const {
  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_, 0));
}
api::opengl::OpenGlUniformBuffer::~OpenGlUniformBuffer() {
  GL_CALL(glDeleteBuffers(1, &uniform_id_));
}

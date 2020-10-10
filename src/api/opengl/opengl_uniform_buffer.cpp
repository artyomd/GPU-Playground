//
// Created by artyomd on 3/10/20.
//

#include "src/api/opengl/opengl_uniform_buffer.hpp"

#include <GL/glew.h>

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlUniformBuffer::OpenGlUniformBuffer(int length, int binding_point, api::ShaderType shader_stage)
    : UniformBuffer(binding_point, shader_stage),
      length_(length) {
  GL_CALL(glGenBuffers(1, &render_id_));
}

void api::opengl::OpenGlUniformBuffer::Update(const void *data) {
  GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, render_id_));
  GL_CALL(glBufferData(GL_UNIFORM_BUFFER, this->length_, data, GL_DYNAMIC_DRAW));
  GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
void api::opengl::OpenGlUniformBuffer::Bind() const {
  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_, render_id_));
}
void api::opengl::OpenGlUniformBuffer::Unbind() const {
  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_, 0));
}
api::opengl::OpenGlUniformBuffer::~OpenGlUniformBuffer() {
  GL_CALL(glDeleteBuffers(1, &render_id_));
}

//
// Created by artyomd on 3/10/20.
//

#include <GL/glew.h>
#include "GlUniformBuffer.h"
#include "GlUtils.h"
#include "GlRenderingPipeline.h"

api::GlUniformBuffer::GlUniformBuffer(int length, int binding_point, api::ShaderType shader_stage)
    : UniformBuffer(binding_point, shader_stage),
      length_(length) {
  GL_CALL(glGenBuffers(1, &render_id_));
}

void api::GlUniformBuffer::Update(const void *data) {
  GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, render_id_));
  GL_CALL(glBufferData(GL_UNIFORM_BUFFER, this->length_, data, GL_DYNAMIC_DRAW));
  GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
void api::GlUniformBuffer::Bind() const {
  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_, render_id_));
}
void api::GlUniformBuffer::Unbind() const {
  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_, 0));
}
api::GlUniformBuffer::~GlUniformBuffer() {
  GL_CALL(glDeleteBuffers(1, &render_id_));

}
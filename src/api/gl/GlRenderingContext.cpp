//
// Created by artyomd on 12/31/19.
//

#include <iostream>
#include <vendor/glm/ext.hpp>
#include "GlRenderingContext.h"
#include "GlVertexBuffer.h"
#include "GlVertexBinding.h"
#include "GlIndexBuffer.h"
#include "GlRenderingPipeline.h"
#include "GlShader.h"
#include "GlUniformBuffer.h"
#include "GlUtils.h"
#include "GlRenderingPipelineLayout.h"

namespace api {
GlRenderingContext::GlRenderingContext() : RenderingContext() {}

IndexBuffer *GlRenderingContext::CreateIndexBuffer(const void *data, unsigned int size, DataType type) {
  return new GlIndexBuffer(data, size, type);
}

void GlRenderingContext::FreeIndexBuffer(IndexBuffer *buffer) {
  delete buffer;
}

VertexBuffer *GlRenderingContext::CreateVertexBuffer(const void *data, unsigned int size) {
  return new GlVertexBuffer(data, size);
}

void GlRenderingContext::FreeVertexBuffer(VertexBuffer *buffer) {
  delete buffer;
}

VertexBinding *
GlRenderingContext::CreateVertexBinding(const VertexBuffer *buffer, const VertexBufferLayout *vertex_buffer_layout) {
  return new GlVertexBinding(buffer, vertex_buffer_layout);
}

void GlRenderingContext::FreeVertexBiding(VertexBinding *vertex_binding) {
  delete vertex_binding;
}

RenderingPipeline *GlRenderingContext::CreateGraphicsPipeline(const VertexBinding *vertex_binding,
                                                              const IndexBuffer *index_buffer,
                                                              const Shader *vertex_shader,
                                                              const Shader *fragment_shader,
                                                              const RenderingPipelineLayout *pipeline_layout) {
  return new GlRenderingPipeline(this, vertex_binding, index_buffer, vertex_shader, fragment_shader, pipeline_layout);
}

void GlRenderingContext::FreeGraphicsPipeline(RenderingPipeline *pipeline) {
  delete pipeline;
}

Shader *GlRenderingContext::CreateShader(std::string sipr_v_shader_location, std::string glsl_location,
                                         std::string entry_point_name,
                                         api::ShaderType type) {
  return new GlShader(sipr_v_shader_location, glsl_location, entry_point_name, type);
}

void GlRenderingContext::DeleteShader(Shader *vertex_binding) {
  delete vertex_binding;

}

Uniform *GlRenderingContext::CreateUniformBuffer(int length,
                                                 int binding_point,
                                                 ShaderType shader_stage) {
  return new GlUniformBuffer(length, binding_point, shader_stage);
}
void GlRenderingContext::DeleteUniformBuffer(Uniform *uniform_buffer) {
  delete uniform_buffer;
}
void GlRenderingContext::SetViewportSize(int width, int height) {
  viewport_width_ = width;
  viewport_height_ = height;
  float new_width = 4.0f;
  float new_height = ((float) width*new_width)/(float) height;
  ortho_projection_ = (glm::ortho(-new_width, new_width, -new_height, new_height));
}
int GlRenderingContext::GetViewportWidth() const {
  return viewport_width_;
}
int GlRenderingContext::GetViewportHeight() const {
  return viewport_height_;
}
void GlRenderingContext::WaitForGpuIdle() const {
  GL_CALL(glFinish());
}
RenderingPipelineLayout *GlRenderingContext::CreateRenderingPipelineLayout(const std::vector<Uniform *> &bindings) {
  return new GlRenderingPipelineLayout(bindings);
}
void GlRenderingContext::FreeRenderingPipelineLayout(RenderingPipelineLayout *pipeline_layout) {
  delete pipeline_layout;
}
}
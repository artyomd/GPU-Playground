//
// Created by artyomd on 12/31/19.
//

#include "src/api/gl/GlRenderingContext.h"

#include <glm/ext.hpp>

#include "src/api/gl/GlIndexBuffer.h"
#include "src/api/gl/GlRenderingPipeline.h"
#include "src/api/gl/GlRenderingPipelineLayout.h"
#include "src/api/gl/GlShader.h"
#include "src/api/gl/GlTexture2D.h"
#include "src/api/gl/GlUniformBuffer.h"
#include "src/api/gl/GlUtils.h"
#include "src/api/gl/GlVertexBinding.h"
#include "src/api/gl/GlVertexBuffer.h"

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
                                                              const RenderingPipelineLayout *pipeline_layout,
                                                              const RenderingPipelineLayoutConfig &config) {
  return new GlRenderingPipeline(this,
                                 vertex_binding,
                                 index_buffer,
                                 vertex_shader,
                                 fragment_shader,
                                 pipeline_layout,
                                 config);
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

UniformBuffer *GlRenderingContext::CreateUniformBuffer(int length,
                                                       int binding_point,
                                                       ShaderType shader_stage) {
  return new GlUniformBuffer(length, binding_point, shader_stage);
}

void GlRenderingContext::DeleteUniformBuffer(UniformBuffer *uniform_buffer) {
  delete uniform_buffer;
}
Texture2D *GlRenderingContext::CreateTexture2D(std::string image_path, int binding_point, ShaderType shader_stage) {
  return new GlTexture2D(image_path, binding_point, shader_stage);
}
void GlRenderingContext::DeleteTexture2D(Texture2D *texture_2_d) {
  delete texture_2_d;
}
void GlRenderingContext::SetViewportSize(int width, int height) {
  viewport_width_ = width;
  viewport_height_ = height;
  float new_width = 4.0f;
  float new_height = ((float) width * new_width) / (float) height;
  ortho_projection_ = (glm::ortho(-new_width, new_width, -new_height, new_height));
  prespective_projection_ =
      glm::perspective(glm::radians(45.0f), (float) viewport_width_ / (float) viewport_height_, 0.1f, 10.0f);
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
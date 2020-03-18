//
// Created by artyomd on 12/31/19.
//

#include <iostream>
#include "GlRenderingContext.h"
#include "GlVertexBuffer.h"
#include "GlVertexBinding.h"
#include "GlIndexBuffer.h"
#include "GlRenderingPipeline.h"
#include "GlShader.h"
#include "GlUniformBuffer.h"

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
                                                              const UniformBuffer *shader_properties) {
  return new GlRenderingPipeline(vertex_binding, index_buffer, vertex_shader, fragment_shader, shader_properties);
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
}
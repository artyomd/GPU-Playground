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

std::shared_ptr<IndexBuffer> GlRenderingContext::CreateIndexBuffer(const void *data, unsigned int size, DataType type) {
  return std::make_shared<GlIndexBuffer>(data, size, type);
}

std::shared_ptr<VertexBuffer> GlRenderingContext::CreateVertexBuffer(const void *data, unsigned int size) {
  return std::make_shared<GlVertexBuffer>(data, size);
}

std::shared_ptr<VertexBinding>
GlRenderingContext::CreateVertexBinding(std::shared_ptr<VertexBuffer> buffer,
                                        std::shared_ptr<VertexBufferLayout> vertex_buffer_layout) {
  return std::make_shared<GlVertexBinding>(buffer, vertex_buffer_layout);
}

std::shared_ptr<RenderingPipeline> GlRenderingContext::CreateGraphicsPipeline(
    std::shared_ptr<VertexBinding> vertex_binding,
    std::shared_ptr<IndexBuffer> index_buffer,
    std::shared_ptr<Shader> vertex_shader,
    std::shared_ptr<Shader> fragment_shader,
    std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
    RenderingPipelineLayoutConfig config) {
  return std::make_shared<GlRenderingPipeline>(shared_from_this(),
                                               vertex_binding,
                                               index_buffer,
                                               vertex_shader,
                                               fragment_shader,
                                               pipeline_layout,
                                               config);
}

std::shared_ptr<Shader> GlRenderingContext::CreateShader(std::string sipr_v_shader_location,
                                                         std::string glsl_location,
                                                         std::string entry_point_name,
                                                         api::ShaderType type) {
  return std::make_shared<GlShader>(sipr_v_shader_location, glsl_location, entry_point_name, type);
}

std::shared_ptr<UniformBuffer> GlRenderingContext::CreateUniformBuffer(int length,
                                                                       int binding_point,
                                                                       ShaderType shader_stage) {
  return std::make_shared<GlUniformBuffer>(length, binding_point, shader_stage);
}

std::shared_ptr<Texture2D> GlRenderingContext::CreateTexture2D(std::string image_path,
                                                               int binding_point,
                                                               ShaderType shader_stage) {
  return std::make_shared<GlTexture2D>(image_path, binding_point, shader_stage);
}

void GlRenderingContext::SetViewportSize(int width, int height) {
  viewport_width_ = width;
  viewport_height_ = height;
  float new_width = 4.0f;
  float new_height = ((float) width * new_width) / (float) height;
  orthographic_projection_ = (glm::ortho(-new_width, new_width, -new_height, new_height));
  perspective_projection_ =
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
std::shared_ptr<RenderingPipelineLayout> GlRenderingContext::CreateRenderingPipelineLayout(
    const std::vector<std::shared_ptr<Uniform>> &bindings) {
  return std::make_shared<GlRenderingPipelineLayout>(bindings);
}
}
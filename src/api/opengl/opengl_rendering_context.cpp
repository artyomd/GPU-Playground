//
// Created by artyomd on 12/31/19.
//

#include "src/api/opengl/opengl_rendering_context.hpp"

#include <glm/ext.hpp>

#include "src/api/opengl/opengl_index_buffer.hpp"
#include "src/api/opengl/opengl_rendering_pipeline.hpp"
#include "src/api/opengl/opengl_rendering_pipeline_layout.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_texture_2d.hpp"
#include "src/api/opengl/opengl_uniform_buffer.hpp"
#include "src/api/opengl/opengl_utils.hpp"
#include "src/api/opengl/opengl_vertex_binding.hpp"
#include "src/api/opengl/opengl_vertex_buffer.hpp"

api::opengl::OpenGlRenderingContext::OpenGlRenderingContext() : RenderingContext() {}

std::shared_ptr<api::IndexBuffer> api::opengl::OpenGlRenderingContext::CreateIndexBuffer(const void *data,
                                                                                         unsigned int size,
                                                                                         DataType type) {
  return std::make_shared<OpenGlIndexBuffer>(data, size, type);
}

std::shared_ptr<api::VertexBuffer> api::opengl::OpenGlRenderingContext::CreateVertexBuffer(const void *data,
                                                                                           unsigned int size) {
  return std::make_shared<OpenGlVertexBuffer>(data, size);
}

std::shared_ptr<api::VertexBinding>
api::opengl::OpenGlRenderingContext::CreateVertexBinding(std::shared_ptr<VertexBuffer> buffer,
                                                         std::shared_ptr<VertexBufferLayout> vertex_buffer_layout) {
  return std::make_shared<OpenGlVertexBinding>(buffer, vertex_buffer_layout);
}

std::shared_ptr<api::RenderingPipeline> api::opengl::OpenGlRenderingContext::CreateGraphicsPipeline(
    std::shared_ptr<VertexBinding> vertex_binding,
    std::shared_ptr<IndexBuffer> index_buffer,
    std::shared_ptr<Shader> vertex_shader,
    std::shared_ptr<Shader> fragment_shader,
    std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
    RenderingPipelineLayoutConfig config) {
  return std::make_shared<OpenGlRenderingPipeline>(shared_from_this(),
                                                   vertex_binding,
                                                   index_buffer,
                                                   vertex_shader,
                                                   fragment_shader,
                                                   pipeline_layout,
                                                   config);
}

std::shared_ptr<api::Shader> api::opengl::OpenGlRenderingContext::CreateShader(std::string sipr_v_shader_location,
                                                                               std::string entry_point_name,
                                                                               api::ShaderType type) {
  return std::make_shared<OpenGlShader>(sipr_v_shader_location, entry_point_name, type);
}

std::shared_ptr<api::UniformBuffer> api::opengl::OpenGlRenderingContext::CreateUniformBuffer(int length,
                                                                                             int binding_point,
                                                                                             ShaderType shader_stage) {
  return std::make_shared<OpenGlUniformBuffer>(length, binding_point, shader_stage);
}

std::shared_ptr<api::Texture2D> api::opengl::OpenGlRenderingContext::CreateTexture2D(std::string image_path,
                                                                                     int binding_point,
                                                                                     api::ShaderType shader_stage) {
  return std::make_shared<OpenglTexture2D>(image_path, binding_point, shader_stage);
}

void api::opengl::OpenGlRenderingContext::SetViewportSize(int width, int height) {
  viewport_width_ = width;
  viewport_height_ = height;
  float new_width = 4.0f;
  float new_height = ((float) width * new_width) / (float) height;
  orthographic_projection_ = (glm::ortho(-new_width, new_width, -new_height, new_height));
  perspective_projection_ =
      glm::perspective(glm::radians(45.0f), (float) viewport_width_ / (float) viewport_height_, 0.1f, 10.0f);
}
int api::opengl::OpenGlRenderingContext::GetViewportWidth() const {
  return viewport_width_;
}
int api::opengl::OpenGlRenderingContext::GetViewportHeight() const {
  return viewport_height_;
}
void api::opengl::OpenGlRenderingContext::WaitForGpuIdle() const {
  GL_CALL(glFinish());
}
std::shared_ptr<api::RenderingPipelineLayout> api::opengl::OpenGlRenderingContext::CreateRenderingPipelineLayout(
    const std::vector<std::shared_ptr<Uniform>> &bindings) {
  return std::make_shared<OpenGlRenderingPipelineLayout>(bindings);
}

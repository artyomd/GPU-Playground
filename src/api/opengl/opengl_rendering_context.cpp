//
// Created by artyomd on 12/31/19.
//

#include "src/api/opengl/opengl_rendering_context.hpp"

#include "src/api/opengl/opengl_buffer.hpp"
#include "src/api/opengl/opengl_index_buffer.hpp"
#include "src/api/opengl/opengl_rendering_pipeline.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_texture_2d.hpp"
#include "src/api/opengl/opengl_utils.hpp"
#include "src/api/opengl/opengl_vertex_buffer.hpp"

api::opengl::OpenGlRenderingContext::OpenGlRenderingContext() : RenderingContext() {}

std::shared_ptr<api::Buffer> api::opengl::OpenGlRenderingContext::CreateBuffer(size_t size_in_bytes) {
  return std::make_shared<api::opengl::OpenGlBuffer>(size_in_bytes);
}

std::shared_ptr<api::IndexBuffer> api::opengl::OpenGlRenderingContext::CreateIndexBuffer(unsigned int count,
                                                                                         api::DataType type) {
  return std::make_shared<api::opengl::OpenGlIndexBuffer>(count, type);
}

std::shared_ptr<api::VertexBuffer> api::opengl::OpenGlRenderingContext::CreateVertexBuffer(size_t size_in_bytes,
                                                                                           api::VertexBufferLayout layout) {
  return std::make_shared<OpenGlVertexBuffer>(size_in_bytes, layout);
}

std::shared_ptr<api::Shader> api::opengl::OpenGlRenderingContext::CreateShader(std::string sipr_v_shader_location,
                                                                               std::string entry_point_name,
                                                                               api::ShaderType type) {
  return std::make_shared<OpenGlShader>(sipr_v_shader_location, entry_point_name, type);
}

std::shared_ptr<api::Texture2D> api::opengl::OpenGlRenderingContext::CreateTexture2D() {
  return std::make_shared<OpenglTexture2D>();
}

std::shared_ptr<api::RenderingPipeline> api::opengl::OpenGlRenderingContext::CreateGraphicsPipeline(
    std::shared_ptr<VertexBuffer> vertex_buffer,
    std::shared_ptr<IndexBuffer> index_buffer,
    std::shared_ptr<Shader> vertex_shader,
    std::shared_ptr<Shader> fragment_shader,
    api::RenderingPipelineConfig config) {
  return std::make_shared<api::opengl::OpenGlRenderingPipeline>(vertex_buffer,
                                                                index_buffer,
                                                                vertex_shader,
                                                                fragment_shader,
                                                                config);
}

//void api::opengl::OpenGlRenderingContext::SetViewportSize(int width, int height) {
//  viewport_width_ = width;
//  viewport_height_ = height;
//  float new_width = 4.0f;
//  float new_height = ((float) width * new_width) / (float) height;
//  orthographic_projection_ = (glm::ortho(-new_width, new_width, -new_height, new_height));
//  perspective_projection_ =
//      glm::perspective(glm::radians(45.0f), (float) viewport_width_ / (float) viewport_height_, 0.1f, 10.0f);
//}

void api::opengl::OpenGlRenderingContext::WaitForGpuIdle() const {
  GL_CALL(glFinish());
}

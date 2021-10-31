//
// Created by artyomd on 12/31/19.
//

#include "src/api/opengl/opengl_rendering_context.hpp"

#include "src/api/opengl/opengl_buffer.hpp"
#include "src/api/opengl/opengl_rendering_pipeline.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_texture_2d.hpp"
#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenGlRenderingContext::OpenGlRenderingContext() : RenderingContext() {}

std::shared_ptr<api::Buffer> api::opengl::OpenGlRenderingContext::CreateBuffer(size_t size_in_bytes,
                                                                               api::BufferUsage,
                                                                               api::MemoryType) {
  return std::make_shared<api::opengl::OpenGlBuffer>(size_in_bytes);
}

std::shared_ptr<api::Shader> api::opengl::OpenGlRenderingContext::CreateShader(std::string sipr_v_shader_location,
                                                                               std::string entry_point_name,
                                                                               api::ShaderType type) {
  return std::make_shared<OpenGlShader>(sipr_v_shader_location, entry_point_name, type);
}

std::shared_ptr<api::Texture2D> api::opengl::OpenGlRenderingContext::CreateTexture2D(uint32_t width,
                                                                                     uint32_t height,
                                                                                     PixelFormat pixel_format) {
  return std::make_shared<OpenglTexture2D>(width, height, pixel_format);
}

std::shared_ptr<api::RenderingPipeline> api::opengl::OpenGlRenderingContext::CreateGraphicsPipeline(
    std::shared_ptr<Shader> vertex_shader,
    std::shared_ptr<Shader> fragment_shader,
    const api::VertexBufferLayout &vbl,
    api::RenderingPipelineConfig config) {
  return std::make_shared<api::opengl::OpenGlRenderingPipeline>(vertex_shader, fragment_shader, vbl, config);
}

void api::opengl::OpenGlRenderingContext::WaitForGpuIdle() const {
  GL_CALL(glFinish());
}

std::shared_ptr<api::RenderingContext> api::CreateOpenGlRenderingContext() {
  return std::make_shared<api::opengl::OpenGlRenderingContext>();
}

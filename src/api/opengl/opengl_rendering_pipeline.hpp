//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <GL/glew.h>
#include <map>

#include "src/api/rendering_pipeline.hpp"

#include "src/api/opengl/opengl_index_buffer.hpp"
#include "src/api/opengl/opengl_rendering_context.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_vertex_buffer.hpp"
#include "src/api/opengl/opengl_texture_2d.hpp"

namespace api::opengl {
class OpenGlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = 0;

  std::shared_ptr<OpenGlVertexBuffer> vertex_buffer_;
  std::shared_ptr<OpenGlIndexBuffer> index_buffer_;
  std::shared_ptr<OpenGlShader> vertex_shader_;
  std::shared_ptr<OpenGlShader> fragment_shader_;

  GLenum draw_mode_ = 0;
  api::RenderingPipelineConfig config_;

  GLsizei width_ = 0;
  GLsizei height_ = 0;

  std::map<GLuint, std::shared_ptr<opengl::OpenGlBuffer>> ubos_{};
  std::map<GLuint, std::shared_ptr<OpenglTexture2D>> textures_{};

 public:
  OpenGlRenderingPipeline(const std::shared_ptr<VertexBuffer> &vertex_buffer,
                          const std::shared_ptr<IndexBuffer> &index_buffer,
                          const std::shared_ptr<Shader> &vertex_shader,
                          const std::shared_ptr<Shader> &fragment_shader,
                          api::RenderingPipelineConfig config);
  void UpdateUniformBuffer(unsigned int binding_point, void *data) override;
  void SetTexture(unsigned int binding_point, std::shared_ptr<api::Texture2D> texture) override;
  void SetViewPort(size_t width, size_t height) override;
  void Render() override;
  ~OpenGlRenderingPipeline() override;
};
}

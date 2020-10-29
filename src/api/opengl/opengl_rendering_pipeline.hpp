//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <GL/glew.h>

#include "src/api/rendering_pipeline.hpp"

#include "src/api/opengl/opengl_index_buffer.hpp"
#include "src/api/opengl/opengl_rendering_context.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_uniform.hpp"
#include "src/api/opengl/opengl_vertex_buffer.hpp"

namespace api::opengl {
class OpenGlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = -1;

  std::shared_ptr<OpenGlVertexBuffer> vertex_buffer_;
  std::shared_ptr<OpenGlIndexBuffer> index_buffer_;
  std::shared_ptr<OpenGlShader> vertex_shader_;
  std::shared_ptr<OpenGlShader> fragment_shader_;

  GLenum draw_mode_ = 0;

  int width_ = 0;
  int height_ = 0;

  std::vector<std::shared_ptr<OpenGlUniform>> uniforms_{};

 public:
  OpenGlRenderingPipeline(const std::shared_ptr<VertexBuffer> &vertex_buffer,
                          const std::shared_ptr<IndexBuffer> &index_buffer,
                          const std::shared_ptr<Shader> &vertex_shader,
                          const std::shared_ptr<Shader> &fragment_shader,
                          api::RenderingPipelineConfig config);

  void AddUniform(std::shared_ptr<api::Uniform> uniform) override;
  void Render() override;
  void ViewportChanged(size_t width, size_t height) override;

  ~OpenGlRenderingPipeline() override;
};
}

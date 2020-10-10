//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <GL/glew.h>

#include "src/api/opengl/opengl_rendering_context.hpp"
#include "src/api/rendering_pipeline.hpp"

namespace api::opengl {
class OpenGlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = -1;
  std::shared_ptr<OpenGlRenderingContext> context_;
 public:
  OpenGlRenderingPipeline(std::shared_ptr<OpenGlRenderingContext> context,
                          std::shared_ptr<VertexBinding> vertex_binding,
                          std::shared_ptr<IndexBuffer> index_buffer,
                          std::shared_ptr<Shader> vertex_shader,
                          std::shared_ptr<Shader> fragment_shader,
                          std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
                          RenderingPipelineLayoutConfig config);

  void Render() override;
  void ViewportChanged() override;

  ~OpenGlRenderingPipeline() override;
};
}
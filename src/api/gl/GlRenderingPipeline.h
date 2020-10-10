//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <GL/glew.h>

#include "src/api/gl/GlRenderingContext.h"
#include "src/api/RenderingPipeline.h"

namespace api {
class GlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = -1;
  std::shared_ptr<GlRenderingContext> context_;
 public:
  GlRenderingPipeline(std::shared_ptr<GlRenderingContext> context,
                      std::shared_ptr<VertexBinding> vertex_binding,
                      std::shared_ptr<IndexBuffer> index_buffer,
                      std::shared_ptr<Shader> vertex_shader,
                      std::shared_ptr<Shader> fragment_shader,
                      std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
                      RenderingPipelineLayoutConfig config = {});

  void Render() override;
  void ViewportChanged() override;

  ~GlRenderingPipeline() override;
};
}
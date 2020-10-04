//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <GL/glew.h>

#include "src/api/gl/GlRenderingContext.h"
#include "src/api/RenderingContext.h"
#include "src/api/RenderingPipeline.h"

namespace api {
class GlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = -1;
  const GlRenderingContext *context_;
 public:
  GlRenderingPipeline(const GlRenderingContext *context,
                      const VertexBinding *vertex_binding,
                      const IndexBuffer *index_buffer,
                      const Shader *vertex_shader,
                      const Shader *fragment_shader,
                      const RenderingPipelineLayout *pipeline_layout,
                      const RenderingPipelineLayoutConfig &config = {});

  void Render() override;
  void ViewportChanged() override;

  ~GlRenderingPipeline() override;
};
}
//
// Created by artyomd on 1/1/20.
//

#pragma once

#include "RenderingPipeline.h"
#include "GlRenderingContext.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <api/RenderingContext.h>

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
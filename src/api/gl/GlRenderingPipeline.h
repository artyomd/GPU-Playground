//
// Created by artyomd on 1/1/20.
//

#pragma once

#include "RenderingPipeline.h"
#include <GL/glew.h>
#include <GL/gl.h>

namespace api {
class GlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = -1;
 public:
  GlRenderingPipeline(const VertexBinding *vertex_binding,
                      const IndexBuffer *index_buffer,
                      const Shader *vertex_shader,
                      const Shader *fragment_shader);

  void Render() override;

  ~GlRenderingPipeline() override;
};
}
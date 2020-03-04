//
// Created by artyomd on 1/1/20.
//
#pragma once

#include "VertexBinding.h"
#include "IndexBuffer.h"
#include "Shader.h"

namespace api {
class RenderingPipeline {
 protected:
  const VertexBinding *vertex_binding_;
  const IndexBuffer *index_buffer_;
  const Shader *vertex_shader_;
  const Shader *fragment_shader_;
 public:
  RenderingPipeline(const VertexBinding *vertex_binding, const IndexBuffer *index_buffer,
                    const Shader *vertex_shader, const Shader *fragment_shader) : vertex_binding_(vertex_binding),
                                                                                  index_buffer_(index_buffer),
                                                                                  vertex_shader_(vertex_shader),
                                                                                  fragment_shader_(fragment_shader) {}

  virtual void Render() = 0;

  virtual ~RenderingPipeline() = default;
};
}

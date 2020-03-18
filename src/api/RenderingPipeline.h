//
// Created by artyomd on 1/1/20.
//
#pragma once

#include "VertexBinding.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Shader.h"

namespace api {
class RenderingPipeline {
 protected:
  const VertexBinding *vertex_binding_;
  const IndexBuffer *index_buffer_;
  const Shader *vertex_shader_;
  const Shader *fragment_shader_;
  const api::UniformBuffer *shader_properties_;
 public:
  RenderingPipeline(const VertexBinding *vertex_binding,
                    const IndexBuffer *index_buffer,
                    const Shader *vertex_shader,
                    const Shader *fragment_shader,
                    const UniformBuffer *shader_properties)
      : vertex_binding_(vertex_binding),
        index_buffer_(index_buffer),
        vertex_shader_(vertex_shader),
        fragment_shader_(fragment_shader),
        shader_properties_(shader_properties) {}

  virtual void Render() = 0;

  virtual void ViewportChanged() = 0;

  virtual ~RenderingPipeline() = default;
};
}

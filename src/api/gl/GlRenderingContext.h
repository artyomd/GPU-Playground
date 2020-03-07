//
// Created by artyomd on 12/31/19.
//
#pragma once

#include <api/RenderingContext.h>

namespace api {
class GlRenderingContext : public RenderingContext {
 public:
  GlRenderingContext();
  IndexBuffer *CreateIndexBuffer(const void *data, unsigned int size, DataType type) override;

  void FreeIndexBuffer(IndexBuffer *buffer) override;

  VertexBuffer *CreateVertexBuffer(const void *data, unsigned int size) override;

  void FreeVertexBuffer(VertexBuffer *buffer) override;

  VertexBinding *CreateVertexBinding(const VertexBuffer *buffer,
                                     const VertexBufferLayout *vertex_buffer_layout) override;

  void FreeVertexBiding(VertexBinding *vertex_binding) override;

  RenderingPipeline *CreateGraphicsPipeline(const VertexBinding *vertex_binding,
                                            const IndexBuffer *index_buffer,
                                            const Shader *vertex_shader,
                                            const Shader *fragment_shader) override;

  void FreeGraphicsPipeline(RenderingPipeline *pipeline) override;

  Shader *CreateShader(std::string sipr_v_shader_location,
                       std::string glsl_location,
                       std::string entry_point_name,
                       api::ShaderType type) override;

  void DeleteShader(Shader *vertex_binding) override;

  void SetOrthoProjection(const glm::mat4x4 &ortho_projection);
};
}
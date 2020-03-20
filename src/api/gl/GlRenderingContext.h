//
// Created by artyomd on 12/31/19.
//
#pragma once

#include <api/RenderingContext.h>

namespace api {
class GlRenderingContext : public RenderingContext {
 private:
  int viewport_width_ = 0;
  int viewport_height_ = 0;
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
                                            const Shader *fragment_shader,
                                            const UniformBuffer *shader_properties) override;

  void FreeGraphicsPipeline(RenderingPipeline *pipeline) override;

  void SetViewportSize(int width, int height) override;
  Shader *CreateShader(std::string sipr_v_shader_location,
                       std::string glsl_location,
                       std::string entry_point_name,
                       api::ShaderType type) override;

  void DeleteShader(Shader *vertex_binding) override;

  UniformBuffer *CreateUniformBuffer(int length, int binding_point, ShaderType shader_stage) override;

  void DeleteUniformBuffer(UniformBuffer *vertex_binding) override;

  [[nodiscard]] int GetViewportWidth() const;

  [[nodiscard]] int GetViewportHeight() const;

  void WaitForGpuIdle() const override;
  ~GlRenderingContext() override = default;
};
}
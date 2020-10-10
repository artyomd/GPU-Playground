//
// Created by artyomd on 12/31/19.
//
#pragma once

#include <memory>
#include "src/api/rendering_context.hpp"

namespace api::opengl {
class OpenGlRenderingContext : public RenderingContext, public std::enable_shared_from_this<OpenGlRenderingContext> {
 private:
  int viewport_width_ = 0;
  int viewport_height_ = 0;
 public:
  OpenGlRenderingContext();
  std::shared_ptr<IndexBuffer> CreateIndexBuffer(const void *data, unsigned int size, DataType type) override;

  std::shared_ptr<VertexBuffer> CreateVertexBuffer(const void *data, unsigned int size) override;

  std::shared_ptr<VertexBinding> CreateVertexBinding(std::shared_ptr<VertexBuffer> buffer,
                                                     std::shared_ptr<VertexBufferLayout> vertex_buffer_layout) override;

  std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_location,
                                       std::string entry_point_name,
                                       api::ShaderType type) override;

  std::shared_ptr<UniformBuffer> CreateUniformBuffer(int length, int binding_point, ShaderType shader_stage) override;

  std::shared_ptr<Texture2D> CreateTexture2D(std::string image_path,
                                             int binding_point,
                                             ShaderType shader_stage) override;

  std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(std::shared_ptr<VertexBinding> vertex_binding,
                                                                 std::shared_ptr<IndexBuffer> index_buffer,
                                                                 std::shared_ptr<Shader> vertex_shader,
                                                                 std::shared_ptr<Shader> fragment_shader,
                                                                 std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
                                                                 RenderingPipelineLayoutConfig config) override;

  std::shared_ptr<RenderingPipelineLayout> CreateRenderingPipelineLayout(const std::vector<std::shared_ptr<Uniform>> &bindings) override;

  void SetViewportSize(int width, int height) override;

  void WaitForGpuIdle() const override;

  [[nodiscard]] int GetViewportWidth() const;

  [[nodiscard]] int GetViewportHeight() const;
};
}

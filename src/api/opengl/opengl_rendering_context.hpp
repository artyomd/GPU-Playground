//
// Created by artyomd on 12/31/19.
//
#pragma once

#include "src/api/rendering_context.hpp"

namespace api::opengl {
class OpenGlRenderingContext : public RenderingContext {
 public:
  OpenGlRenderingContext();
  std::shared_ptr<IndexBuffer> CreateIndexBuffer(unsigned int count, DataType type) override;
  std::shared_ptr<VertexBuffer> CreateVertexBuffer(size_t size_in_bytes, VertexBufferLayout layout) override;
  std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_location,
                                       std::string entry_point_name,
                                       api::ShaderType type) override;
  std::shared_ptr<UniformBuffer> CreateUniformBuffer(size_t size_in_bytes,
                                                     int binding_point,
                                                     ShaderType shader_stage) override;
  std::shared_ptr<Texture2D> CreateTexture2D(std::string image_path,
                                             int binding_point,
                                             ShaderType shader_stage) override;
  std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(std::shared_ptr<VertexBuffer> vertex_binding,
                                                                 std::shared_ptr<IndexBuffer> index_buffer,
                                                                 std::shared_ptr<Shader> vertex_shader,
                                                                 std::shared_ptr<Shader> fragment_shader,
                                                                 RenderingPipelineConfig config) override;
  void WaitForGpuIdle() const override;
};
}

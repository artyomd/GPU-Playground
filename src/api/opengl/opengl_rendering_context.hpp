//
// Created by artyomd on 12/31/19.
//
#pragma once

#include "src/api/rendering_context.hpp"

namespace api::opengl {
class OpenGlRenderingContext : public RenderingContext {
 public:
  OpenGlRenderingContext();
  std::shared_ptr<IndexBuffer> CreateIndexBuffer(uint32_t count, DataType type) override;
  std::shared_ptr<VertexBuffer> CreateVertexBuffer(size_t size_in_bytes, VertexBufferLayout layout) override;
  std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_location,
                                       std::string entry_point_name,
                                       api::ShaderType type) override;

  std::shared_ptr<Buffer> CreateBuffer(size_t size_in_bytes) override;

  std::shared_ptr<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat pixel_format) override;

  std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(std::shared_ptr<VertexBuffer> vertex_binding,
                                                                 std::shared_ptr<IndexBuffer> index_buffer,
                                                                 std::shared_ptr<Shader> vertex_shader,
                                                                 std::shared_ptr<Shader> fragment_shader,
                                                                 RenderingPipelineConfig config) override;
  void WaitForGpuIdle() const override;
};
}

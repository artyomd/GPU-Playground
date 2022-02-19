#pragma once

#include "src/api/rendering_context.hpp"

namespace api::opengl {
class OpenGlRenderingContext : public RenderingContext {
 public:
  OpenGlRenderingContext();

  std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_location,
                                       std::string entry_point_name,
                                       api::ShaderType type) override;

  std::shared_ptr<Buffer> CreateBuffer(size_t size_in_bytes, BufferUsage usage, MemoryType memory) override;

  std::shared_ptr<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat pixel_format) override;

  std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(std::shared_ptr<Shader> vertex_shader,
                                                                 std::shared_ptr<Shader> fragment_shader,
                                                                 const VertexBufferLayout &vbl,
                                                                 RenderingPipelineConfig config) override;

  void WaitForGpuIdle() const override;
};
}

#pragma once

#include "src/api/buffer.hpp"
#include "src/api/rendering_pipeline.hpp"
#include "src/api/shader.hpp"
#include "src/api/texture_2d.hpp"
#include "src/api/vertex_buffer_layout.hpp"

namespace api {
class RenderingContext {
 public:
  RenderingContext() = default;

  virtual std::shared_ptr<Buffer> CreateBuffer(size_t size_in_bytes, BufferUsage usage, MemoryType memory) = 0;

  virtual std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_source,
                                               std::string entry_point_name,
                                               api::ShaderType type) = 0;

  virtual std::shared_ptr<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat pixel_format) = 0;

  virtual std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(
      std::shared_ptr<Shader> vertex_shader,
      std::shared_ptr<Shader> fragment_shader,
      const VertexBufferLayout &vbl,
      RenderingPipelineConfig config) = 0;

  virtual void WaitForGpuIdle() const = 0;

  virtual ~RenderingContext() = default;
};

std::shared_ptr<RenderingContext> CreateOpenGlRenderingContext();
}

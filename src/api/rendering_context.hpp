#pragma once

#include "src/api/buffer.hpp"
#include "src/api/index_buffer.hpp"
#include "src/api/rendering_pipeline.hpp"
#include "src/api/shader.hpp"
#include "src/api/texture_2d.hpp"
#include "src/api/vertex_buffer.hpp"
#include "src/api/vertex_buffer_layout.hpp"

namespace api {
class RenderingContext {
 public:
  RenderingContext() = default;

  virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(unsigned int count, DataType type) = 0;

  virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(size_t size_in_bytes, VertexBufferLayout layout) = 0;

  virtual std::shared_ptr<Buffer> CreateBuffer(size_t size_in_bytes) = 0;

  virtual std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_source,
                                               std::string entry_point_name,
                                               api::ShaderType type) = 0;

  virtual std::shared_ptr<Texture2D> CreateTexture2D(size_t width, size_t height, PixelFormat pixel_format) = 0;

  virtual std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(
      std::shared_ptr<VertexBuffer> vertex_binding,
      std::shared_ptr<IndexBuffer> index_buffer,
      std::shared_ptr<Shader> vertex_shader,
      std::shared_ptr<Shader> fragment_shader,
      RenderingPipelineConfig config) = 0;

  virtual void WaitForGpuIdle() const = 0;

  virtual ~RenderingContext() = default;
};
}

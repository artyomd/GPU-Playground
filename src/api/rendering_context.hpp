#pragma once

#include "src/api/index_buffer.hpp"
#include "src/api/rendering_pipeline.hpp"
#include "src/api/shader.hpp"
#include "src/api/texture_2d.hpp"
#include "src/api/uniform_buffer.hpp"
#include "src/api/vertex_buffer.hpp"

namespace api {
class RenderingContext {
 public:
  RenderingContext() = default;

  virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(unsigned int count, DataType type) = 0;

  virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(size_t size_in_bytes, VertexBufferLayout layout) = 0;

  virtual std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_path,
                                               std::string entry_point_name,
                                               api::ShaderType type) = 0;

  virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(size_t size_in_bytes,
                                                             int binding_point,
                                                             ShaderType shader_stage) = 0;

  virtual std::shared_ptr<Texture2D> CreateTexture2D(std::string image_path,
                                                     int binding_point,
                                                     ShaderType shader_stage) = 0;

  virtual std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(
      std::shared_ptr<VertexBuffer> vertex_binding,
      std::shared_ptr<IndexBuffer> index_buffer,
      std::shared_ptr<Shader> vertex_shader,
      std::shared_ptr<Shader> fragment_shader,
      RenderingPipelineConfig config) = 0;

  virtual void WaitForGpuIdle() const = 0;

//  [[nodiscard]] inline const glm::mat4x4 &GetOrthographicProjection() const {
//    return orthographic_projection_;
//  }
//
//  [[nodiscard]] inline const glm::mat4x4 &GetPerspectiveProjection() const {
//    return perspective_projection_;
//  }

  virtual ~RenderingContext() = default;
};
}

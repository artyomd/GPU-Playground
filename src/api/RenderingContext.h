#pragma once

#include <glm/glm.hpp>

#include "src/api/IndexBuffer.h"
#include "src/api/RenderingPipeline.h"
#include "src/api/Shader.h"
#include "src/api/Texture2D.h"
#include "src/api/UniformBuffer.h"
#include "src/api/VertexBuffer.h"
#include "src/api/VertexBinding.h"

namespace api {
class RenderingContext {
 protected:
  glm::mat4x4 orthographic_projection_{};
  glm::mat4x4 perspective_projection_{};
 public:
  RenderingContext() = default;

  virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(const void *data, unsigned int size, DataType type) = 0;

  virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(const void *data, unsigned int size) = 0;

  virtual std::shared_ptr<VertexBinding> CreateVertexBinding(
      std::shared_ptr<VertexBuffer> buffer,
      std::shared_ptr<VertexBufferLayout> vertex_buffer_layout) = 0;

  virtual std::shared_ptr<Shader> CreateShader(std::string sipr_v_shader_location,
                                               std::string glsl_location,
                                               std::string entry_point_name,
                                               api::ShaderType type) = 0;

  virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(int length,
                                                             int binding_point,
                                                             ShaderType shader_stage) = 0;

  virtual std::shared_ptr<Texture2D> CreateTexture2D(std::string image_path,
                                                     int binding_point,
                                                     ShaderType shader_stage) = 0;

  virtual std::shared_ptr<api::RenderingPipeline> CreateGraphicsPipeline(
      std::shared_ptr<VertexBinding> vertex_binding,
      std::shared_ptr<IndexBuffer> index_buffer,
      std::shared_ptr<Shader> vertex_shader,
      std::shared_ptr<Shader> fragment_shader,
      std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
      RenderingPipelineLayoutConfig config) = 0;

  virtual std::shared_ptr<RenderingPipelineLayout> CreateRenderingPipelineLayout(
      const std::vector<std::shared_ptr<Uniform>> &bindings) = 0;

  virtual void SetViewportSize(int width, int height) = 0;

  virtual void WaitForGpuIdle() const = 0;

  [[nodiscard]] inline const glm::mat4x4 &GetOrthographicProjection() const {
    return orthographic_projection_;
  }

  [[nodiscard]] inline const glm::mat4x4 &GetPerspectiveProjection() const {
    return perspective_projection_;
  }

  virtual ~
  RenderingContext() = default;
};
}
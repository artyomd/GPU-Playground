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
  glm::mat4x4 ortho_projection_{};
  glm::mat4x4 prespective_projection_{};
 public:
  RenderingContext() = default;

  virtual IndexBuffer *CreateIndexBuffer(const void *data, unsigned int size, DataType type) = 0;

  virtual void FreeIndexBuffer(IndexBuffer *buffer) = 0;

  virtual VertexBuffer *CreateVertexBuffer(const void *data, unsigned int size) = 0;

  virtual void FreeVertexBuffer(VertexBuffer *buffer) = 0;

  virtual VertexBinding *CreateVertexBinding(const VertexBuffer *buffer,
                                             const VertexBufferLayout *vertex_buffer_layout) = 0;

  virtual void FreeVertexBiding(VertexBinding *vertex_binding) = 0;

  virtual Shader *CreateShader(std::string sipr_v_shader_location,
                               std::string glsl_location,
                               std::string entry_point_name,
                               api::ShaderType type) = 0;

  virtual void DeleteShader(Shader *vertex_binding) = 0;

  virtual UniformBuffer *CreateUniformBuffer(int length, int binding_point, ShaderType shader_stage) = 0;

  virtual void DeleteUniformBuffer(UniformBuffer *uniform_buffer) = 0;

  virtual Texture2D *CreateTexture2D(std::string image_path, int binding_point, ShaderType shader_stage) = 0;

  virtual void DeleteTexture2D(Texture2D *texture_2_d) = 0;

  virtual api::RenderingPipeline *CreateGraphicsPipeline(const VertexBinding *vertex_binding,
                                                         const IndexBuffer *index_buffer,
                                                         const Shader *vertex_shader,
                                                         const Shader *fragment_shader,
                                                         const RenderingPipelineLayout *pipeline_layout,
                                                         const RenderingPipelineLayoutConfig &config = {}) = 0;

  virtual void FreeGraphicsPipeline(RenderingPipeline *pipeline) = 0;

  virtual RenderingPipelineLayout *CreateRenderingPipelineLayout(const std::vector<Uniform *> &bindings) = 0;

  virtual void FreeRenderingPipelineLayout(RenderingPipelineLayout *pipeline_layout) = 0;

  virtual void SetViewportSize(int width, int height) = 0;

  virtual void WaitForGpuIdle() const = 0;

  [[nodiscard]] const glm::mat4x4 &GetOrthoProjection() const {
    return ortho_projection_;
  }
  [[nodiscard]] const glm::mat4x4 &GetPrespectiveProjection() const {
    return prespective_projection_;
  }

  virtual ~RenderingContext() = default;
};
}
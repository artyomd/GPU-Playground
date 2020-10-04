//
// Created by artyomd on 3/31/20.
//
#pragma once

#include "src/geometry/Triangle.h"
#include "src/geometry/Quad.h"
#include "src/test/TestModel.h"

namespace test {
class TestTexture2D : public TestModel {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer *uniform_buffer_ = nullptr;
  api::Texture2D *texture_2_d_ = nullptr;
  api::RenderingPipelineLayout *pipeline_layout_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  api::VertexBuffer *vertex_buffer_ = nullptr;
  api::IndexBuffer *index_buffer_ = nullptr;
  api::VertexBufferLayout *vertex_buffer_layout_ = nullptr;
  api::VertexBinding *vertex_binding_ = nullptr;
  UniformBufferObjectMvp *ubo_ = new UniformBufferObjectMvp();

 public:
  explicit TestTexture2D(api::Renderer *renderer);

  void OnClear() override;

  void OnRender() override;

  void OnViewportChange() override;

  ~TestTexture2D() override;
};
}
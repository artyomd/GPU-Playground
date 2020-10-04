//
// Created by artyomd on 5/1/20.
//

#pragma once

#include "src/api/Renderer.hpp"
#include "src/geometry/Triangle.h"
#include "src/test/Test.h"
#include "src/test/TestModel.h"

namespace test {
class TestObj : public TestModel {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer *uniform_buffer_ = nullptr;
  api::Texture2D *obj_texture_ = nullptr;
  api::RenderingPipelineLayout *pipeline_layout_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  api::VertexBuffer *vertex_buffer_ = nullptr;
  api::IndexBuffer *index_buffer_ = nullptr;
  api::VertexBufferLayout *vertex_buffer_layout_ = nullptr;
  api::VertexBinding *vertex_binding_ = nullptr;
  UniformBufferObjectMvp *ubo_ = new UniformBufferObjectMvp();

 public:
  explicit TestObj(api::Renderer *renderer);

  void OnClear() override;

  void OnRender() override;

  void OnViewportChange() override;

  ~TestObj() override;
};
}
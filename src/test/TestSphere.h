//
// Created by artyomd on 3/23/20.
//
#pragma once

#include <geometry/GeometryItem.h>
#include "TestModel.h"
namespace test {
class TestSphere : public TestModel {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer *uniform_buffer_ = nullptr;
  api::RenderingPipelineLayout *pipeline_layout_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  geometry::GeometryItem *sphere_ = nullptr;
  UniformBufferObjectMvp *ubo_ = new UniformBufferObjectMvp();

 public:
  explicit TestSphere(api::Renderer *renderer);

  void OnClear() override;

  void OnRender() override;

  void OnViewportChange() override;

  ~TestSphere() override;
};
}
//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "src/api/Renderer.hpp"
#include "src/geometry/Triangle.h"
#include "src/test/TestModel.h"

namespace test {
class TestTriangle : public TestModel {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer *uniform_buffer_ = nullptr;
  api::RenderingPipelineLayout *pipeline_layout_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  geometry::Triangle *triangle_ = nullptr;
  UniformBufferObjectMvp *ubo_ = new UniformBufferObjectMvp();

 public:
  explicit TestTriangle(api::Renderer *renderer);

  void OnClear() override;

  void OnRender() override;

  void OnViewportChange() override;

  ~TestTriangle() override;
};
}

//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "api/Renderer.hpp"
#include "geometry/Triangle.h"
#include "Test.h"
#include "TestModel.h"

namespace test {
class TestTriangle : public TestModel {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer *uniform_buffer_ = nullptr;
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

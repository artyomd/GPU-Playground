//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "api/Renderer.hpp"
#include "geometry/Triangle.h"
#include "Test.h"
#include "TestModel.h"

namespace test {
struct UniformBufferObject {
  alignas(16) glm::mat4 model_;
  alignas(16) glm::mat4 view_;
  alignas(16) glm::mat4 proj_;
};


class TestTriangle : public TestModel {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer* uniform_buffer_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  geometry::Triangle *triangle_ = nullptr;
  UniformBufferObject * ubo_ = new UniformBufferObject();

 public:
  explicit TestTriangle(api::Renderer *renderer);

  void OnClear() override;

  void OnRender() override;

  ~TestTriangle() override;
  void OnViewportChange() override;
};
}

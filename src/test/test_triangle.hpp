//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "src/api/renderer.hpp"
#include "src/geometry/triangle.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestTriangle : public TestModel {
 private:
  std::shared_ptr<api::Shader> vertex_shader_ = nullptr;
  std::shared_ptr<api::Shader> fragment_shader_ = nullptr;
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<api::RenderingPipelineLayout> pipeline_layout_ = nullptr;
  std::shared_ptr<api::RenderingPipeline> pipeline_ = nullptr;
  std::shared_ptr<geometry::Triangle> triangle_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();

 public:
  explicit TestTriangle(std::shared_ptr<api::Renderer> renderer);

  void OnClear() override;

  void OnRender() override;

  void OnViewportChange() override;
};
}

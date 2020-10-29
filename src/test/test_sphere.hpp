//
// Created by artyomd on 3/23/20.
//
#pragma once

#include "src/geometry/geometry_item.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestSphere : public TestModel {
 private:
  std::shared_ptr<api::Shader> vertex_shader_ = nullptr;
  std::shared_ptr<api::Shader> fragment_shader_ = nullptr;
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<geometry::GeometryItem> sphere_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();

 public:
  explicit TestSphere(std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};
}

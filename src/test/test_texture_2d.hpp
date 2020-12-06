//
// Created by artyomd on 3/31/20.
//
#pragma once

#include "src/geometry/triangle.hpp"
#include "src/geometry/quad.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestTexture2D : public TestModel {
 private:
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();

 public:
  explicit TestTexture2D(std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};
}

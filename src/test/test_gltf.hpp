//
// Created by artyomd on 12/21/20.
//
#pragma once

#include "src/test/test_model.hpp"

namespace test {
class TestGltf : public TestModel {
 private:
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();
 public:
  explicit TestGltf(std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};
}
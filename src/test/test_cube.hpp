//
// Created by artyomd on 5/23/20.
//
#pragma once

#include "src/api/shader.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestCube : public TestModel {
 private:
  std::shared_ptr<api::Shader> vertex_shader_ = nullptr;
  std::shared_ptr<api::Shader> fragment_shader_ = nullptr;
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<api::VertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<api::IndexBuffer> index_buffer_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();
 public:
  explicit TestCube(std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};

}

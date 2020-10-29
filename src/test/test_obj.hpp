//
// Created by artyomd on 5/1/20.
//

#pragma once

#include "src/geometry/triangle.hpp"
#include "src/test/test.h"
#include "src/test/test_model.hpp"

namespace test {
class TestObj : public TestModel {
 private:
  std::shared_ptr<api::Shader> vertex_shader_ = nullptr;
  std::shared_ptr<api::Shader> fragment_shader_ = nullptr;
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<api::Texture2D> obj_texture_ = nullptr;
  std::shared_ptr<api::VertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<api::IndexBuffer> index_buffer_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();

 public:
  explicit TestObj(std::shared_ptr<api::RenderingContext> renderer);

  void OnRender() override;
};
}

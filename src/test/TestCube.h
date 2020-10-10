//
// Created by artyomd on 5/23/20.
//
#pragma once

#include "src/api/Shader.h"
#include "src/test/TestModel.h"

namespace test {
class TestCube : public TestModel {
 private:
  std::shared_ptr<api::Shader> vertex_shader_ = nullptr;
  std::shared_ptr<api::Shader> fragment_shader_ = nullptr;
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<api::RenderingPipelineLayout> pipeline_layout_ = nullptr;
  std::shared_ptr<api::RenderingPipeline> pipeline_ = nullptr;
  std::shared_ptr<api::VertexBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<api::IndexBuffer> index_buffer_ = nullptr;
  std::shared_ptr<api::VertexBufferLayout> vertex_buffer_layout_ = std::make_shared<api::VertexBufferLayout>();
  std::shared_ptr<api::VertexBinding> vertex_binding_ = nullptr;
  std::shared_ptr<UniformBufferObjectMvp> ubo_ = std::make_shared<UniformBufferObjectMvp>();
 public:
  explicit TestCube(std::shared_ptr<api::Renderer> renderer);

  void OnClear() override;

  void OnRender() override;

  void OnViewportChange() override;
};

}

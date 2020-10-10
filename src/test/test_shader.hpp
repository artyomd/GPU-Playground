//
// Created by artyomd on 3/21/20.
//
#pragma once

#include "src/geometry/quad.hpp"
#include "src/test/test.h"

namespace test {
struct UniformBufferObjectShader {
  alignas(8) float screen_width = 0;
  alignas(4) float screen_height = 0;
  alignas(4) float time = 0;
};
class TestShader : public Test {
 private:
  std::shared_ptr<api::Shader> vertex_shader_ = nullptr;
  std::shared_ptr<api::Shader> fragment_shader_ = nullptr;
  std::shared_ptr<api::UniformBuffer> uniform_buffer_ = nullptr;
  std::shared_ptr<api::RenderingPipelineLayout> pipeline_layout_ = nullptr;
  std::shared_ptr<api::RenderingPipeline> pipeline_ = nullptr;
  std::shared_ptr<geometry::Quad> quad_ = nullptr;
  std::shared_ptr<UniformBufferObjectShader> ubo_ = std::make_shared<UniformBufferObjectShader>();

  void UpdateUniformBufferScreenSize();
 public:
  explicit TestShader(std::shared_ptr<api::Renderer> renderer, std::shared_ptr<api::Shader> fragment_shader);

  void OnUpdate(float delta_time) override;

  void OnRender() override;

  void OnImGuiRender() override;

  void OnViewportChange() override;
};
}
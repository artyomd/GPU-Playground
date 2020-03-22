//
// Created by artyomd on 3/21/20.
//
#pragma once

#include "Test.h"
#include "Quad.h"

namespace test {
struct UniformBufferObjectShader {
  alignas(8) float screen_width_ = 0;
  alignas(4) float screen_height_ = 0;
  alignas(4) float time_ = 0;
};
class TestShader : public Test {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::UniformBuffer *uniform_buffer_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  geometry::Quad *quad_ = nullptr;
  UniformBufferObjectShader *ubo_ = new UniformBufferObjectShader();

  void UpdateUniformBufferScreenSize();
 public:
  explicit TestShader(api::Renderer *renderer, api::Shader *fragment_shader);

  void OnUpdate(float delta_time) override;

  void OnRender() override;

  void OnImGuiRender() override;

  void OnViewportChange() override;

  ~TestShader() override;
};
}
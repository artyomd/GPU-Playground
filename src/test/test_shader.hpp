//
// Created by artyomd on 3/21/20.
//
#pragma once

#include "src/geometry/quad.hpp"
#include "src/shaders/shaders.hpp"
#include "src/test/test.h"

namespace test {
struct UniformBufferObjectShader {
  alignas(8) float screen_width = 0;
  alignas(4) float screen_height = 0;
  alignas(4) float time = 0;
};
class TestShader : public Test {
 private:
  UniformBufferObjectShader ubo_{};
  size_t index_count_ = 0;
  std::shared_ptr<api::RenderingPipeline> pipeline_ = nullptr;

 public:
  explicit TestShader(std::shared_ptr<api::RenderingContext> rendering_context,
                      std::string fragment_shader);

  void OnUpdate(float delta_time) override;

  void OnRender() override;

  void OnImGuiRender() override;
  void OnViewportChange(uint32_t width, uint32_t height) override;
};
}  // namespace test

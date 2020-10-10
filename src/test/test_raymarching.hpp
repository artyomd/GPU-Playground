//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestRaymarching : public TestShader {
 public:
  explicit TestRaymarching(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer),
                 renderer_->GetContext()->
                     CreateShader("../res/shader/compiled/raymarched_reflections.spv",
                                  "../res/shader/raymarched_reflections.glsl",
                                  "main",
                                  api::ShaderType::SHADER_TYPE_FRAGMENT)) {}
};
}
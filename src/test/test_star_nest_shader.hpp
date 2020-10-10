//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestStarNestShader : public TestShader {
 public:
  explicit TestStarNestShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer),
                 renderer_->GetContext()->
                     CreateShader("../res/shader/compiled/start_nest_fragment_shader.spv",
                                  "../res/shader/start_nest_fragment_shader.glsl",
                                  "main",
                                  api::ShaderType::SHADER_TYPE_FRAGMENT)) {}
};
}
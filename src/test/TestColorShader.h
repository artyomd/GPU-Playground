//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/TestShader.h"

namespace test {
class TestColorShader : public TestShader {
 public:
  explicit TestColorShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer),
                 renderer_->GetContext()->
                     CreateShader("../res/shader/compiled/color_fragment_shader.spv",
                                  "../res/shader/color_fragment_shader.glsl",
                                  "main",
                                  api::ShaderType::SHADER_TYPE_FRAGMENT)) {}
};
}
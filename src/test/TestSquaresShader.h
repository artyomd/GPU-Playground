//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/TestShader.h"

namespace test {
class TestSquaresShader : public TestShader {
 public:
  explicit TestSquaresShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer),
                 renderer_->GetContext()->
                     CreateShader("../res/shader/compiled/squares_fragment_shader.spv",
                                  "../res/shader/squares_fragment_shader.glsl",
                                  "main",
                                  api::ShaderType::SHADER_TYPE_FRAGMENT)) {}
};
}
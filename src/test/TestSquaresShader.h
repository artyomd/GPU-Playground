//
// Created by artyomd on 3/22/20.
//

#pragma once

#include "src/test/TestShader.h"

namespace test {
class TestSquaresShader : public TestShader {
 public:
  explicit TestSquaresShader(api::Renderer *renderer) :
      TestShader(renderer,
                 renderer->GetRenderingContext()->
                     CreateShader("../res/shader/compiled/squares_fragment_shader.spv",
                                  "../res/shader/squares_fragment_shader.glsl",
                                  "main",
                                  api::SHADER_TYPE_FRAGMENT)) {}
};
}
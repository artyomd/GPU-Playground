//
// Created by artyomd on 3/22/20.
//

#pragma once

#include "src/test/TestShader.h"

namespace test {
class TestSeascapeShader : public TestShader {
 public:
  explicit TestSeascapeShader(api::Renderer *renderer) :
      TestShader(renderer,
                 renderer->GetRenderingContext()->
                     CreateShader("../res/shader/compiled/seascape_fragment_shader.spv",
                                  "../res/shader/seascape_fragment_shader.glsl",
                                  "main",
                                  api::SHADER_TYPE_FRAGMENT)) {}
};
}
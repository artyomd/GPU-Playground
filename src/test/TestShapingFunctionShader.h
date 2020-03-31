//
// Created by artyomd on 3/22/20.
//

#pragma once

#include "TestShader.h"

namespace test {
class TestShapingFunctionShader : public TestShader {
 public:
  explicit TestShapingFunctionShader(api::Renderer *renderer) :
      TestShader(renderer,
                 renderer->GetRenderingContext()->
                     CreateShader("../res/shader/compiled/shaping_function_fragment_shader.spv",
                                  "../res/shader/shaping_function_fragment_shader.glsl",
                                  "main",
                                  api::SHADER_TYPE_FRAGMENT)) {}
};
}
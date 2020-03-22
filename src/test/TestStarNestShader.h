//
// Created by artyomd on 3/22/20.
//
#pragma once

#include "TestShader.h"

namespace test {
class TestStarNestShader : public TestShader {
 public:
  explicit TestStarNestShader(api::Renderer *renderer) :
      TestShader(renderer,
                 renderer->GetRenderingContext()->
                     CreateShader("../res/shader/compiled/start_nest_fragment_shader.spv",
                                  "../res/shader/start_nest_fragment_shader.glsl",
                                  "main",
                                  api::SHADER_TYPE_FRAGMENT)) {}
};
}
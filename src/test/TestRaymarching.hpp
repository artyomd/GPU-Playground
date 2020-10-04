//
// Created by artyomd on 3/22/20.
//
#pragma once

#include "src/test/TestShader.h"

namespace test {
class TestRaymarching : public TestShader {
 public:
  explicit TestRaymarching(api::Renderer *renderer) :
      TestShader(renderer,
                 renderer->GetRenderingContext()->
                     CreateShader("../res/shader/compiled/raymarched_reflections.spv",
                                  "../res/shader/raymarched_reflections.glsl",
                                  "main",
                                  api::SHADER_TYPE_FRAGMENT)) {}
};
}
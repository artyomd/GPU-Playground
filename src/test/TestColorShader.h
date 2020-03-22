//
// Created by artyomd on 3/22/20.
//
#pragma once

#include "TestShader.h"
namespace test {
class TestColorShader : public TestShader {
 public:
  explicit TestColorShader(api::Renderer *renderer) :
      TestShader(renderer,
                 renderer->GetRenderingContext()->
                     CreateShader("../res/shader/compiled/color_fragment_shader.spv",
                                  "../res/shader/color_fragment_shader.glsl",
                                  "main",
                                  api::SHADER_TYPE_FRAGMENT)) {}
};
}
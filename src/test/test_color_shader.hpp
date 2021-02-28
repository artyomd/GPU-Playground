//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestColorShader : public TestShader {
 public:
  explicit TestColorShader(std::shared_ptr<api::RenderingContext> rendering_context) :
      TestShader(std::move(rendering_context), {
#include SHADER(color_fragment_shader)
      }) {}
};
}

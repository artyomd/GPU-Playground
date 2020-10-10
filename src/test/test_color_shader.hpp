//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestColorShader : public TestShader {
 public:
  explicit TestColorShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer), "../res/shader/compiled/color_fragment_shader.spv") {}
};
}

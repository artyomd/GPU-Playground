//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestShapingFunctionShader : public TestShader {
 public:
  explicit TestShapingFunctionShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer), "../res/shader/compiled/shaping_function_fragment_shader.spv") {}
};
}

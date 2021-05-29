//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestShapingFunctionShader : public TestShader {
 public:
  explicit TestShapingFunctionShader(std::shared_ptr<api::RenderingContext> rendering_context) :
      TestShader(std::move(rendering_context), shaping_function_fragment_shader) {}
};
}

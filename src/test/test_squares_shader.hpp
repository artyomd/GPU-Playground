//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestSquaresShader : public TestShader {
 public:
  explicit TestSquaresShader(
      std::shared_ptr<api::RenderingContext> rendering_context)
      : TestShader(std::move(rendering_context), squares_fragment_shader) {}
};
}  // namespace test

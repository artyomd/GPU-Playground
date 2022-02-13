//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestSeascapeShader : public TestShader {
 public:
  explicit TestSeascapeShader(
      std::shared_ptr<api::RenderingContext> rendering_context)
      : TestShader(std::move(rendering_context), seascape_fragment_shader) {}
};
}  // namespace test

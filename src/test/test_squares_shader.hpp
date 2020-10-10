//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestSquaresShader : public TestShader {
 public:
  explicit TestSquaresShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer), "../res/shader/compiled/squares_fragment_shader.spv") {}
};
}

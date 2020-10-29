//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestStarNestShader : public TestShader {
 public:
  explicit TestStarNestShader(std::shared_ptr<api::RenderingContext> rendering_context) :
      TestShader(std::move(rendering_context), "../res/shader/compiled/start_nest_fragment_shader.spv") {}
};
}

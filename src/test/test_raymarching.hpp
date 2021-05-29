//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestRaymarching : public TestShader {
 public:
  explicit TestRaymarching(std::shared_ptr<api::RenderingContext> rendering_context) :
      TestShader(std::move(rendering_context), raymarched_reflections) {}
};
}

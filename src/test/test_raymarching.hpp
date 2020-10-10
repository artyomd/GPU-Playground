//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestRaymarching : public TestShader {
 public:
  explicit TestRaymarching(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer), "../res/shader/compiled/raymarched_reflections.spv") {}
};
}

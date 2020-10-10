//
// Created by artyomd on 3/22/20.
//
#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestStarNestShader : public TestShader {
 public:
  explicit TestStarNestShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer), "../res/shader/compiled/start_nest_fragment_shader.spv") {}
};
}

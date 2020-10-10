//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/test_shader.hpp"

namespace test {
class TestSeascapeShader : public TestShader {
 public:
  explicit TestSeascapeShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer), "../res/shader/compiled/seascape_fragment_shader.spv") {}
};
}

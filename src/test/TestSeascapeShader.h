//
// Created by artyomd on 3/22/20.
//

#pragma once

#include <utility>

#include "src/test/TestShader.h"

namespace test {
class TestSeascapeShader : public TestShader {
 public:
  explicit TestSeascapeShader(std::shared_ptr<api::Renderer> renderer) :
      TestShader(std::move(renderer),
                 renderer_->GetContext()->
                     CreateShader("../res/shader/compiled/seascape_fragment_shader.spv",
                                  "../res/shader/seascape_fragment_shader.glsl",
                                  "main",
                                  api::ShaderType::SHADER_TYPE_FRAGMENT)) {}
};
}
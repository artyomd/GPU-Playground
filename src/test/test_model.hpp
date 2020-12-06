//
// Created by artyomd on 3/8/20.
//

#pragma once

#include "src/test/test.h"

#include <glm/glm.hpp>
#include <utility>

namespace test {
struct UniformBufferObjectMvp {
  alignas(16) glm::mat4 model = glm::mat4(1.0f);
  alignas(16) glm::mat4 view = glm::mat4(1.0f);
  alignas(16) glm::mat4 proj = glm::mat4(1.0f);
};

class TestModel : public Test {
 private:
  glm::vec3 translation_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 rotate_ = glm::vec3(0.0f, 0.0f, 0.0f);
  float max_side_ = 2.0f;

 protected:
  glm::mat4 ComputeModelMatrix();
  glm::mat4 orthographic_projection_{};
  bool lock_projection_ = false;
  glm::mat4 perspective_projection_ = glm::mat4(1.0);

 public:
  explicit TestModel(std::shared_ptr<api::RenderingContext> rendering_context) : Test(std::move(rendering_context)) {};

  void OnImGuiRender() override;

  void OnViewportChange(size_t width, size_t height) override;

  ~TestModel() override = default;
};
}

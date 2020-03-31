//
// Created by artyomd on 3/8/20.
//

#pragma once

#include <iostream>
#include "Test.h"

namespace test {
struct UniformBufferObjectMvp {
  alignas(16) glm::mat4 model_;
  alignas(16) glm::mat4 view_;
  alignas(16) glm::mat4 proj_;
};

class TestModel : public Test {
 private:
  glm::vec3 translation_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 rotate_ = glm::vec3(0.0f, 0.0f, 0.0f);
  float max_side_ = 2.0f;

 protected:
  glm::mat4 ComputeModelMatrix();

 public:
  explicit TestModel(api::Renderer *renderer) : Test(renderer) {};

  void OnImGuiRender() override;

  ~TestModel() override = default;
};
}
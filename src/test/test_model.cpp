//
// Created by artyomd on 3/8/20.
//

#include "src/test/test_model.hpp"

#include <cmath>
#include <glm/ext.hpp>
#include <imgui/imgui.h>

void test::TestModel::OnImGuiRender() {
  ImGui::SliderFloat2("translation", &translation_.x, -max_side_, max_side_);
  ImGui::SliderFloat2("scale", &scale_.x, 0, 20);
  ImGui::SliderFloat3("rotate", &rotate_.x, 0, 360);
}

glm::mat4 test::TestModel::ComputeModelMatrix() {
  glm::mat4 model = glm::scale(glm::mat4(1.0f), scale_);
  model = glm::rotate(model, glm::radians(rotate_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotate_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotate_.z), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, translation_);
  return model;
}

void test::TestModel::OnViewportChange(uint32_t width, uint32_t height) {
  Test::OnViewportChange(width, height);
  float new_width = 4.0f;
  float new_height = (static_cast<float>(width) * new_width) / static_cast<float>(height);
  max_side_ = std::fmax(new_width / 2, new_height / 2);
  if (!lock_projection_) {
    orthographic_projection_ = glm::ortho(-new_width, new_width, -new_height, new_height);
    perspective_projection_ =
        glm::perspective(glm::radians(45.0f), (static_cast<float>(width) / static_cast<float>(height)), 0.1f, 10.0f);
  }
  if (pipeline_ != nullptr) {
    pipeline_->SetViewPort(size_[0], size_[1]);
  }
}


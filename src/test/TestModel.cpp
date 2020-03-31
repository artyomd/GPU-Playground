//
// Created by artyomd on 3/8/20.
//

#include <vendor/glm/ext.hpp>
#include <vendor/imgui/imgui.h>
#include "TestModel.h"

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


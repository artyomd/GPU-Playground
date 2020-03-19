//
// Created by Artyom Dangizyan on 11/17/18.
//

#include "TestClearColor.h"
#include "imgui/imgui.h"

test::TestClearColor::TestClearColor(api::Renderer *renderer) : Test(renderer) {

}
void test::TestClearColor::OnClear() {
  renderer_->SetClearColor(color_[0], color_[1], color_[2], color_[3]);
}
void test::TestClearColor::OnImGuiRender() {
  ImGui::ColorEdit4("Clear color", color_);
}
test::TestClearColor::~TestClearColor() {
  renderer_->SetClearColor(0, 0, 0, 1);
}

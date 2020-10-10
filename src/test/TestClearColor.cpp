//
// Created by Artyom Dangizyan on 11/17/18.
//

#include "src/test/TestClearColor.h"

#include <imgui/imgui.h>
#include <utility>

test::TestClearColor::TestClearColor(std::shared_ptr<api::Renderer> renderer) : Test(std::move(renderer)) {

}
void test::TestClearColor::OnClear() {
  renderer_->SetClearColor(color_);
}
void test::TestClearColor::OnImGuiRender() {
  ImGui::ColorEdit4("Clear color", reinterpret_cast<float *>(&color_));
}
test::TestClearColor::~TestClearColor() {
  renderer_->SetClearColor({0, 0, 0, 255});
}

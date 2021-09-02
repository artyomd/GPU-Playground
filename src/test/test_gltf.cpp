//
// Created by artyomd on 12/21/20.
//

#include "src/test/test_gltf.hpp"

#include <utility>
#include <imgui.h>
#include <algorithm>

#include "src/geometry/gltf_model.hpp"

test::TestGltf::TestGltf(std::shared_ptr<api::RenderingContext> rendering_context) :
    Test(std::move(rendering_context)),
    model_(geometry::GltfModel(rendering_context_, "../res/models/Duck.gltf")) {
  scenes_names_ = model_.GetScenes();
  selected_scene_ = model_.GetDefaultSceneIndex();
  if (selected_scene_ != -1) {
    model_.LoadScene(selected_scene_);
  }
}

void test::TestGltf::OnRender() {
  model_.Render();
}

void test::TestGltf::OnViewportChange(uint32_t width, uint32_t height) {
  Test::OnViewportChange(width, height);
  model_.SetViewport(width, height);
}

void test::TestGltf::OnImGuiRender() {
  Test::OnImGuiRender();

  ImGui::Text("Scenes:");
  if (ImGui::BeginListBox("##Scenes",
                          ImVec2(-FLT_MIN,
                                 std::min(static_cast<float>(scenes_names_.size()), 7.0f)
                                     * ImGui::GetTextLineHeightWithSpacing()))) {
    for (size_t n = 0; n < scenes_names_.size(); n++) {
      const bool kIsSelected = (selected_scene_ == static_cast<int>(n));
      if (ImGui::Selectable(scenes_names_[n].c_str(), kIsSelected)) {
        selected_scene_ = static_cast<int>(n);
        model_.LoadScene(selected_scene_);
      }
      if (kIsSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndListBox();
  }
}


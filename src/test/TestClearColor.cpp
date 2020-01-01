//
// Created by Artyom Dangizyan on 11/17/18.
//

#include "TestClearColor.h"
#include "api/Renderer.h"
#include "imgui/imgui.h"

void test::TestClearColor::onClear() {
    renderer->setClearColor(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
}

void test::TestClearColor::onImGuiRender() {
    ImGui::ColorEdit4("Clear color", m_Color);
}

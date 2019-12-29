//
// Created by Artyom Dangizyan on 11/17/18.
//

#include "TestClearColor.h"
#include "api/Renderer.h"
#include "imgui/imgui.h"

test::TestClearColor::TestClearColor() : m_Color{0.2f, 0.3f, 0.8f, 1.0f} {

}

void test::TestClearColor::onClear(RenderingContext *context) {
    context->setClearColor(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
}

void test::TestClearColor::onImGuiRender() {
    ImGui::ColorEdit4("Clear color", m_Color);
}

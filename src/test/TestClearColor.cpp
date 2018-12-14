//
// Created by Artyom Dangizyan on 11/17/18.
//

#include "TestClearColor.h"
#include "Renderer.h"
#include "imgui/imgui.h"

test::TestClearColor::TestClearColor() : m_Color{0.2f, 0.3f, 0.8f, 1.0f} {

}

void test::TestClearColor::onRender() {
    GLCall(glClearColor(m_Color[0], m_Color[1], m_Color[2], m_Color[3]));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void test::TestClearColor::onImGuiRender() {
    ImGui::ColorEdit4("Clear color", m_Color);
}

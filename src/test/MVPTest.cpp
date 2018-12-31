//
// Created by artyomd on 12/30/18.
//

#include <vendor/imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include "MVPTest.h"

test::MVPTest::MVPTest() {
    projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -2.0f, 2.0f);
    maxSide = 2.0f;
    translation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rotate = glm::vec3(0.0f, 0.0f, 0.0f);
}

void test::MVPTest::onImGuiRender() {
    ImGui::SliderFloat2("translation", &translation.x, -maxSide, maxSide);
    ImGui::SliderFloat2("scale", &scale.x, 0, 20);
    ImGui::SliderFloat3("rotate", &rotate.x, 0, 360);
}

glm::mat4 test::MVPTest::computeMVP() {
    glm::mat4 model = glm::scale(glm::mat4(1.0f), scale);
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, translation);

    return projection * model;
}

void test::MVPTest::onWindowSizeChanged(int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    float newWidth = 2.0f;
    float newHeight = (height * newWidth) / width;
    maxSide = std::max(newWidth, newHeight);
    projection = glm::ortho(-newWidth, newWidth, -newHeight, newHeight, -maxSide, maxSide);
}

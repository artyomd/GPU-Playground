//
// Created by artyomd on 12/30/18.
//

#include <vendor/imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "TestMVP.h"

test::TestMVP::TestMVP(api::Renderer *renderer) : Test(renderer) {
    projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -4.0f, 4.0f);
    maxSide = 4.0f;
    translation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rotate = glm::vec3(0.0f, 0.0f, 0.0f);
}

void test::TestMVP::onImGuiRender() {
    ImGui::SliderFloat2("translation", &translation.x, -maxSide, maxSide);
    ImGui::SliderFloat2("scale", &scale.x, 0, 20);
    ImGui::SliderFloat3("rotate", &rotate.x, 0, 360);
}

glm::mat4 test::TestMVP::computeMVP() {
    glm::mat4 model = glm::scale(glm::mat4(1.0f), scale);
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, translation);

    return projection * model;
}

void test::TestMVP::onWindowSizeChanged(int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    float newWidth = 4.0f;
    float newHeight = (height * newWidth) / width;
    maxSide = std::max(newWidth, newHeight);
    projection = glm::ortho(-newWidth, newWidth, -newHeight, newHeight, -maxSide, maxSide);
}

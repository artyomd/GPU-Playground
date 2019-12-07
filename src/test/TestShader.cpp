//
// Created by artyomd on 1/2/19.
//
#include <chrono>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/imgui/imgui.h>
#include <api/ShaderPropertyFloat2.h>
#include "TestShader.h"

test::TestShader::TestShader(const std::string &vertexShaderName, const std::string &fragmentShaderName) {
    geometry::Point point0 = {-1.0f, -1.0f, 0.0f};
    geometry::Point point1 = {1.0f, -1.0f, 0.0f};
    geometry::Point point2 = {-1.0f, 1.0f, 0.0f};
    geometry::Point point3 = {1.0f, 1.0f, 0.0f};

    quad = new geometry::Quad(point0, point1, point2, point3);

    shader = new Shader("../res/shader/" + vertexShaderName + ".glsl",
                        "../res/shader/" + fragmentShaderName + ".glsl");
    timeUniform = new ShaderPropertyFloat1(0);
}

void test::TestShader::onRender() {
    shader->bind();
    shader->setUniform("u_time", timeUniform);
    quad->render(*shader);
    shader->unbind();
}

void test::TestShader::onUpdate(float deltaTime) {
    timeUniform->setValue(timeUniform->getValue() + deltaTime);
}

test::TestShader::~TestShader() {
    delete quad;
    delete shader;
}

void test::TestShader::onImGuiRender() {
    if (ImGui::Button("Reload")) {
        shader->recompile();
    }
}

void test::TestShader::onWindowSizeChanged(int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    ShaderProperty *property = new ShaderPropertyFloat2(width, height);
    shader->bind();
    shader->setUniform("u_screenSize", property);
    shader->unbind();
}

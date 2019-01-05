//
// Created by artyomd on 1/2/19.
//
#include <chrono>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include "TestShader.h"

test::TestShader::TestShader(std::string vertexShaderName, std::string fragmentShaderName) {
    Point point0 = {-1.0f, -1.0f, 0.0f};
    Point point1 = {1.0f, -1.0f, 0.0f};
    Point point2 = {-1.0f, 1.0f, 0.0f};
    Point point3 = {1.0f, 1.0f, 0.0f};

    quad = new Quad(point0, point1, point2, point3);

    shader = new Shader("../res/shader/" + vertexShaderName + ".glsl",
                        "../res/shader/" + fragmentShaderName + ".glsl");
}

void test::TestShader::onRender() {
    shader->bind();
    shader->setUniform1f("u_time", time);
    time += deltaTime;
    quad->render(*shader);
}

test::TestShader::~TestShader() {
    delete quad;
    delete shader;
}

void test::TestShader::onWindowSizeChanged(int width, int height) {
    if (width == 0 || height == 0) {
        return;
    }
    shader->bind();
    shader->setUniform2f("u_screenSize", width, height);
    shader->unbind();
}

//
// Created by Artyom Dangizyan on 2018-11-29.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ShaderPropertyMatrix4f.h>
#include "TestTriangle.h"

namespace test {

    TestTriangle::TestTriangle() : TestMVP() {

        Point point0 = {-0.5f, -0.5f, 0.0f, 1, 1, 0, 0};
        Point point1 = {0.5f, -0.5f, 0.0f, 1, 0, 1, 0};
        Point point2 = {0.0f, 0.5f, 0.0f, 1, 0, 0, 1};

        triangle = new Triangle(point0, point1, point2);

        shader = new Shader("../res/shader/default_mvp_color_vertex_shader.glsl", "../res/shader/default_color_fragment_shader.glsl");
    }

    void TestTriangle::onRender() {
        shader->bind();
        glm::mat4 mvp = computeMVP();
        auto property = new ShaderPropertyMatrix4f(mvp);
        shader->setUniform("u_MVP", property);
        delete property;
        triangle->render(*shader);
        shader->unbind();
    }

    TestTriangle::~TestTriangle() {
        delete shader;
        delete triangle;
    }
}
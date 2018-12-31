//
// Created by Artyom Dangizyan on 2018-11-29.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "TestTriangle.h"

namespace test {

    TestTriangle::TestTriangle() : MVPTest() {

        Point point0 = {-0.5f, -0.5f, 0.0f, 1, 1, 0, 0};
        Point point1 = {0.5f, -0.5f, 0.0f, 1, 0, 1, 0};
        Point point2 = {0.0f, 0.5f, 0.0f, 1, 0, 0, 1};

        triangle = new Triangle(point0, point1, point2);

        shader = new Shader("../res/shader/triangle_vertex.glsl", "../res/shader/triangle_fragment.glsl");
    }

    void TestTriangle::onRender() {
        shader->bind();
        glm::mat4 mvp = computeMVP();
        shader->setUniformMatrix4f("u_MVP", mvp);
        triangle->render(*shader);
    }

    TestTriangle::~TestTriangle() {
        delete shader;
        delete triangle;
    }
}
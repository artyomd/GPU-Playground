//
// Created by Artyom Dangizyan on 2018-11-29.
//

#include <glm/gtc/matrix_transform.hpp>
#include "TestTriangle.h"
#include "../Renderer.h"

namespace test {

    TestTriangle::TestTriangle() : Test() {

        Point point0 = {-0.5f, -0.5f, 0.0f, 1, 1, 0, 0};
        Point point1 = {0.5f, -0.5f, 0.0f, 1, 0, 1, 0};
        Point point2 = {0.0f, 0.5f, 0.0f, 1, 0, 0, 1};

        m_triangle = new Triangle(point0, point1, point2);

        m_shader = new Shader("res/shader/triangle_vertex.glsl", "res/shader/triangle_fragment.glsl");
        m_shader->bind();

        auto projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f);

        m_shader->setUniformMatrix4f("u_MVP", projection);

        m_shader->unbind();
    }

    void TestTriangle::onRender() {
        m_triangle->render(*m_shader);
    }

    TestTriangle::~TestTriangle() {
        delete m_shader;
        delete m_triangle;
    }
}
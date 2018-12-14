//
// Created by Artyom Dangizyan on 2018-11-30.
//

#include <glm/gtc/matrix_transform.hpp>
#include "TestSphere.h"
#include "Renderer.h"

namespace test {
    TestSphere::TestSphere() : Test() {
        // m_sphere = new Sphere(-1);

        sphere = new SpiralSphere(4.0f);
        m_shader = new Shader("../res/shader/sphere_vertex.glsl", "../res/shader/sphere_fragment.glsl");
        m_shader->bind();
        projection = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1000.0f, 1000.0f);
        m_shader->unbind();
    }

    void TestSphere::onRender() {
        m_shader->bind();
        auto mvp = glm::rotate(projection, glm::radians(rotation), glm::vec3(1.0f, 1.0f, 1.0f));
        rotation += 1;
        m_shader->setUniformMatrix4f("u_MVP", projection);
        sphere->render(*m_shader);
    }

    TestSphere::~TestSphere() {
        delete m_shader;
        delete sphere;
    }
}
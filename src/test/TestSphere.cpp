//
// Created by Artyom Dangizyan on 2018-11-30.
//

#include <glm/gtc/matrix_transform.hpp>
#include "TestSphere.h"

namespace test {
    TestSphere::TestSphere() : TestMVP() {
        sphere = new StackedSphere(2.0f);
        shader = new Shader("../res/shader/default_mvp_color_vertex_shader.glsl", "../res/shader/default_color_fragment_shader.glsl");
    }

    void TestSphere::onRender() {
        shader->bind();
        auto mvp = computeMVP();
        shader->setUniformMatrix4f("u_MVP", mvp);
        sphere->render(*shader);
    }

    TestSphere::~TestSphere() {
        delete shader;
        delete sphere;
    }
}
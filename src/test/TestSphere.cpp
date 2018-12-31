//
// Created by Artyom Dangizyan on 2018-11-30.
//

#include <glm/gtc/matrix_transform.hpp>
#include "TestSphere.h"

namespace test {
    TestSphere::TestSphere() : MVPTest() {
        sphere = new StackedSphere(2.0f);
        shader = new Shader("../res/shader/sphere_vertex.glsl", "../res/shader/sphere_fragment.glsl");
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
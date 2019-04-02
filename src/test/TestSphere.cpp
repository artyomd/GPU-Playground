//
// Created by Artyom Dangizyan on 2018-11-30.
//

#include <glm/gtc/matrix_transform.hpp>
#include <ShaderPropertyMatrix4f.h>
#include "TestSphere.h"

namespace test {
    TestSphere::TestSphere() : TestMVP() {
        sphere = new StackedSphere(2.0f);
        shader = new Shader("../res/shader/default_mvp_color_vertex_shader.glsl",
                            "../res/shader/default_color_fragment_shader.glsl");
    }

    void TestSphere::onRender() {
        shader->bind();
        auto mvp = computeMVP();
        auto property = new ShaderPropertyMatrix4f(mvp);
        shader->setUniform("u_MVP", property);
        delete property;
        sphere->render(*shader);
        shader->unbind();
    }

    TestSphere::~TestSphere() {
        delete shader;
        delete sphere;
    }
}
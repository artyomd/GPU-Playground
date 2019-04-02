//
// Created by artyomd on 1/5/19.
//

#pragma once

#include <glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include "TestShader.h"

namespace test {
    class TestStarNestShader : public TestShader {
    public:
        TestStarNestShader() : TestShader("default_empty_vertex_shader", "start_nest_fragment_shader") {}
    };
}
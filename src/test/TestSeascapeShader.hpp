//
// Created by artyomd on 1/5/19.
//

#pragma once

#include <glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include "TestShader.h"

namespace test {
    class TestSeascapeShader : public TestShader {
    public:
        TestSeascapeShader() : TestShader("default_empty_vertex_shader", "seascape_fragment_shader") {}
    };
}
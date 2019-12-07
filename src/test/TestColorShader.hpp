//
// Created by artyomd on 1/2/19.
//

#pragma once

#include <api/Shader.h>
#include <geometry/Quad.h>
#include "Test.h"
#include "TestShader.h"

namespace test {
    class TestColorShader : public TestShader {
    public:
        TestColorShader() : TestShader("default_empty_vertex_shader", "color_fragment_shader") {}
    };
}



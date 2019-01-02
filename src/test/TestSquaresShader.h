//
// Created by artyomd on 1/2/19.
//

#pragma once

#include <Shader.h>
#include <geometry/Quad.h>
#include "Test.h"
#include "TestShader.h"

namespace test {
    class TestSquaresShader : public TestShader {
    public:
        TestSquaresShader() : TestShader("default_empty_vertex_shader", "squares_fragment_shader") {}
    };
}



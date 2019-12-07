//
// Created by artyomd on 1/2/19.
//

#pragma once

#include <api/Shader.h>
#include <geometry/Quad.h>
#include "Test.h"
#include "TestShader.h"

namespace test {
    class TestShapingFunctionShader : public TestShader {
    public:
        TestShapingFunctionShader() : TestShader("default_empty_vertex_shader", "shaping_function_fragment_shader") {}
    };
}



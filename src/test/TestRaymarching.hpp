//
// Created by artyomd on 12/3/19.
//
#pragma once

#include <glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include "TestShader.h"

namespace test {
    class TestRaymarching : public TestShader {
    public:
        TestRaymarching() : TestShader("default_empty_vertex_shader", "raymarched_reflections") {}
    };
}
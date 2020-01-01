//
// Created by artyomd on 1/1/20.
//
#pragma once

#include "VertexBinding.h"
#include "Shader.h"

namespace api {
    class RenderingPipeline {
    private:
        VertexBinding *vertexBinding;
        Shader *vertexShader;
        Shader *fragmentShader;
    };
}

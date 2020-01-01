//
// Created by Artyom Dangizyan on 10/8/18.
//
#pragma once

#include <GL/glew.h>
#include "api/gl/GlVertexBinding.h"
#include "Shader.h"
#include "IndexBuffer.h"

#define GLCall(x) GLClearError;\
    x;\
    assert(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

class Renderer {
public:
    static void draw(const GlVertexBinding &va, const Shader &shader, const IndexBuffer &ib);

    static void clear();
    
};

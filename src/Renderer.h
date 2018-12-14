//
// Created by Artyom Dangizyan on 10/8/18.
//
#pragma once

#include <GL/glew.h>
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"

#define GLCall(x) GLClearError;\
    x;\
    assert(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char *function, const char *file, int line);

class Renderer {
public:
    static void draw(const VertexArray &va, const Shader &shader, const IndexBuffer &ib);

    static void clear();

    static void clearColor(float r, float g, float b, float a);
};

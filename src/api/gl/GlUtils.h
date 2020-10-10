//
// Created by artyomd on 1/6/20.
//

#pragma once

#include <cassert>
#include <GL/glew.h>

#include "src/api/DataType.h"
#include "src/api/Shader.h"

#define GL_CALL(x) GlClearError;\
    x;\
    assert(GlLogCall(#x, __FILE__, __LINE__))

void GlClearError();

bool GlLogCall(const char *function, const char *file, int line);

GLint GetGlType(api::DataType type);

int GetShaderGlType(api::ShaderType shader_type);

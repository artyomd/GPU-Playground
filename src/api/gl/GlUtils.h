//
// Created by artyomd on 1/6/20.
//

#pragma once

#include <cassert>

#define GL_CALL(x) GlClearError;\
    x;\
    assert(GlLogCall(#x, __FILE__, __LINE__))

void GlClearError();

bool GlLogCall(const char *function, const char *file, int line);
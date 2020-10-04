//
// Created by artyomd on 1/6/20.
//

#include <GL/glew.h>
#include <iostream>
#include "GlUtils.h"

void GlClearError() {
  while (glGetError()!=GL_NO_ERROR);
}

bool GlLogCall(const char *function, const char *file, int line) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL error] (" << error << "): "
              << function << ""
              << file << ":"
              << line << std::endl;
    return false;
  }
  return true;
}
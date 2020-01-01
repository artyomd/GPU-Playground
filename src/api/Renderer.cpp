//
// Created by Artyom Dangizyan on 10/8/18.
//

#include <iostream>
#include "Renderer.h"

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char *file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << "): "
                  << function << ""
                  << file << ":"
                  << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::draw(const GlVertexBinding &va, const Shader &shader, const IndexBuffer &ib) {
    shader.bind();
    va.bind();
    ib.bind();

    GLCall(glDrawElements(GL_TRIANGLE_STRIP, ib.getCount(), GL_UNSIGNED_INT, nullptr));

    ib.unbind();
    va.unbind();
    shader.unbind();
}

void Renderer::clear() {
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

}

//
// Created by artyomd on 12/30/19.
//

#include <GL/glew.h>
#include <api/Renderer.h>
#include "GlVertexBuffer.h"

namespace api {
    GlVertexBuffer::GlVertexBuffer(const void *data, unsigned int size) {
        GLCall(glGenBuffers(1, &m_RendererId));
        assert(m_RendererId != 0);
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererId));
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    GlVertexBuffer::~GlVertexBuffer() {
        GLCall(glDeleteBuffers(1, &m_RendererId));
    }

    void GlVertexBuffer::bind() const {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererId));
    }

    void GlVertexBuffer::unbind() const {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

}
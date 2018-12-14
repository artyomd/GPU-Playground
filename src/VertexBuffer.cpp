//
// Created by Artyom Dangizyan on 10/8/18.
//

#include <GL/glew.h>
#include <assert.h>
#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
    GLCall(glGenBuffers(1, &m_RendererId));
    assert(m_RendererId != 0);
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererId));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RendererId));
}

void VertexBuffer::bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererId));
}

void VertexBuffer::unbind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

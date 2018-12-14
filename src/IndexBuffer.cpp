//
// Created by Artyom Dangizyan on 10/8/18.
//

#include "IndexBuffer.h"

#include <GL/glew.h>
#include <assert.h>
#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const void *data, unsigned int count) : m_Count(count) {
    GLCall(glGenBuffers(1, &m_RendererId));
    assert(m_RendererId != 0);
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RendererId));
}

void IndexBuffer::bind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId));
}

void IndexBuffer::unbind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
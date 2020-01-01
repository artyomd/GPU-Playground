//
// Created by Artyom Dangizyan on 10/21/18.
//

#include "GlVertexBinding.h"
#include "Renderer.h"
#include "GlVertexBuffer.h"

namespace api {
    GlVertexBinding::GlVertexBinding() {
        GLCall(glGenVertexArrays(1, &m_RendererId));

    }

    GlVertexBinding::~GlVertexBinding() {
        GLCall(glDeleteVertexArrays(1, &m_RendererId));
    }

    void GlVertexBinding::addBuffer(const VertexBuffer *vb, const VertexAttributeDescription *layout) {
        GLCall(glBindVertexArray(m_RendererId));
        dynamic_cast<const GlVertexBuffer *>(vb)->bind();
        const auto &elements = layout->getElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); i++) {
            const auto &element = elements[i];
            GLCall(glEnableVertexAttribArray(i));
            GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout->getStride(),
                                         reinterpret_cast<void *> (offset)));
            offset += element.count * VertexAttribute::getSizeOfType(element.type);
        }
    }

    void GlVertexBinding::bind() const {
        GLCall(glBindVertexArray(m_RendererId));
    }

    void GlVertexBinding::unbind() const {
        GLCall(glBindVertexArray(0));
    }
}
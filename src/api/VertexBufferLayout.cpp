//
// Created by Artyom Dangizyan on 10/22/18.
//

#include "VertexBufferLayout.h"

template<>
void VertexBufferLayout::Push<float>(unsigned int count) {
    elements.push_back({GL_FLOAT, count, GL_FALSE});
    stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
}

template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
    elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
}

template<>
void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
    elements.push_back({GL_BYTE, count, GL_TRUE});
    stride += count * VertexBufferElement::getSizeOfType(GL_BYTE);
}
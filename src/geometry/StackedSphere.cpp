//
// Created by Artyom Dangizyan on 2018-12-05.
//

#include <cmath>
#include <GL/glew.h>
#include "StackedSphere.h"
#include "Renderer.h"

StackedSphere::StackedSphere(const float radius, const unsigned int stacks, const unsigned int slices) {
    for (unsigned int stackNumber = 0; stackNumber <= stacks; ++stackNumber) {
        for (unsigned int sliceNumber = 0; sliceNumber < slices; ++sliceNumber) {
            auto theta = static_cast<float>(stackNumber * M_PI / stacks);
            auto phi = static_cast<float>(sliceNumber * 2 * M_PI / slices);
            float sinTheta = std::sin(theta);
            float sinPhi = std::sin(phi);
            float cosTheta = std::cos(theta);
            float cosPhi = std::cos(phi);
            Point point = {radius * cosPhi * sinTheta, radius * sinPhi * sinTheta, radius * cosTheta, 1.0f, 1.0f, 0.0f,
                           0.0f};
            m_geometryData.push_back(point);
        }
    }
    for (unsigned int stackNumber = 0; stackNumber < stacks; ++stackNumber) {
        for (unsigned int sliceNumber = 0; sliceNumber <= slices; ++sliceNumber) {
            m_indexData.push_back(static_cast<unsigned short &&>((stackNumber * slices) + (sliceNumber % slices)));
            m_indexData.push_back(static_cast<unsigned short &&>(((stackNumber + 1) * slices) + (sliceNumber % slices)));
        }
    }

    VertexBuffer vertexBuffer(&m_geometryData[0], static_cast<unsigned int>(7 * m_geometryData.size() * sizeof(float)));

    m_vertexArray = new VertexArray();

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(4);
    m_vertexArray->addBuffer(&vertexBuffer, &layout);

    m_indexBuffer = new IndexBuffer(&m_indexData[0], static_cast<unsigned int>(m_indexData.size()));

    m_vertexArray->unbind();
    vertexBuffer.unbind();
    m_indexBuffer->unbind();
}

void StackedSphere::render(Shader &shader) const {
    Renderer::draw(*m_vertexArray, shader, *m_indexBuffer);
}
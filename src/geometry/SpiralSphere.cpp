//
// Created by Artyom Dangizyan on 2018-12-05.
//

#include <cmath>
#include <GL/glew.h>
#include "SpiralSphere.h"
#include "Renderer.h"


SpiralSphere::SpiralSphere(const float radius, const unsigned int loops, const unsigned int segmentsPerLoop) {
    for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
        float theta = 0;
        auto phi = static_cast<float>(loopSegmentNumber * 2 * M_PI / segmentsPerLoop);
        float sinTheta = std::sin(theta);
        float sinPhi = std::sin(phi);
        float cosTheta = std::cos(theta);
        float cosPhi = std::cos(phi);
        Point point = {radius * cosPhi * sinTheta, radius * sinPhi * sinTheta, radius * cosTheta, 1.0f, 0.0f, 0.0f,
                       0.0f};
        m_geometryData.push_back(point);

    }
    for (unsigned int loopNumber = 0; loopNumber <= loops; ++loopNumber) {
        for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
            auto theta = static_cast<float>((loopNumber * M_PI / loops) +
                                            ((M_PI * loopSegmentNumber) / (segmentsPerLoop * loops)));
            if (loopNumber == loops) {
                theta = static_cast<float>(M_PI);
            }
            auto phi = static_cast<float>(loopSegmentNumber * 2 * M_PI / segmentsPerLoop);
            float sinTheta = std::sin(theta);
            float sinPhi = std::sin(phi);
            float cosTheta = std::cos(theta);
            float cosPhi = std::cos(phi);
            Point point = {radius * cosPhi * sinTheta, radius * sinPhi * sinTheta, radius * cosTheta, 1.0f, 1.0f, 1.0f,
                           1.0f};
            m_geometryData.push_back(point);

        }
    }
    for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
        m_indexData.push_back(static_cast<unsigned short &&>(loopSegmentNumber));
        m_indexData.push_back(static_cast<unsigned short &&>(segmentsPerLoop + loopSegmentNumber));
    }
    for (unsigned int loopNumber = 0; loopNumber < loops; ++loopNumber) {
        for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
            m_indexData.push_back(
                    static_cast<unsigned short &&>(((loopNumber + 1) * segmentsPerLoop) + loopSegmentNumber));
            m_indexData.push_back(
                    static_cast<unsigned short &&>(((loopNumber + 2) * segmentsPerLoop) + loopSegmentNumber));
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

void SpiralSphere::render(Shader &shader) const {
    Renderer::draw(*m_vertexArray, shader, *m_indexBuffer);
}
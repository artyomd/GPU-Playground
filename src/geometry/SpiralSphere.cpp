//
// Created by Artyom Dangizyan on 2018-12-05.
//

#include <cmath>
#include "SpiralSphere.h"
#include "api/Renderer.h"

namespace geometry {
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
            geometryData.push_back(point);

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
                Point point = {radius * cosPhi * sinTheta, radius * sinPhi * sinTheta, radius * cosTheta, 1.0f, 1.0f,
                               1.0f,
                               1.0f};
                geometryData.push_back(point);

            }
        }
        for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
            indexData.push_back(static_cast<unsigned short &&>(loopSegmentNumber));
            indexData.push_back(static_cast<unsigned short &&>(segmentsPerLoop + loopSegmentNumber));
        }
        for (unsigned int loopNumber = 0; loopNumber < loops; ++loopNumber) {
            for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
                indexData.push_back(
                        static_cast<unsigned short &&>(((loopNumber + 1) * segmentsPerLoop) + loopSegmentNumber));
                indexData.push_back(
                        static_cast<unsigned short &&>(((loopNumber + 2) * segmentsPerLoop) + loopSegmentNumber));
            }
        }

        VertexBuffer vertexBuffer(&geometryData[0], static_cast<unsigned int>(7 * geometryData.size() * sizeof(float)));

        vertexArray = new VertexArray();

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        vertexArray->addBuffer(&vertexBuffer, &layout);

        indexBuffer = new IndexBuffer(&indexData[0], static_cast<unsigned int>(indexData.size()));

        vertexArray->unbind();
        vertexBuffer.unbind();
        indexBuffer->unbind();
    }

    void SpiralSphere::render(Shader &shader) const {
        Renderer::draw(*vertexArray, shader, *indexBuffer);
    }
}
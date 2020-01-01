//
// Created by Artyom Dangizyan on 2018-12-05.
//

#include <cmath>
#include "StackedSphere.h"
#include "api/Renderer.h"

namespace geometry {
    StackedSphere::StackedSphere(const float radius, const unsigned int stacks, const unsigned int slices) {
        for (unsigned int stackNumber = 0; stackNumber <= stacks; ++stackNumber) {
            for (unsigned int sliceNumber = 0; sliceNumber < slices; ++sliceNumber) {
                auto theta = static_cast<float>(stackNumber * M_PI / stacks);
                auto phi = static_cast<float>(sliceNumber * 2 * M_PI / slices);
                float sinTheta = std::sin(theta);
                float sinPhi = std::sin(phi);
                float cosTheta = std::cos(theta);
                float cosPhi = std::cos(phi);
                Point point = {radius * cosPhi * sinTheta, radius * sinPhi * sinTheta, radius * cosTheta, 1.0f, 1.0f,
                               0.0f,
                               0.0f};
                geometryData.push_back(point);
            }
        }
        for (unsigned int stackNumber = 0; stackNumber < stacks; ++stackNumber) {
            for (unsigned int sliceNumber = 0; sliceNumber <= slices; ++sliceNumber) {
                indexData.push_back(static_cast<unsigned short &&>((stackNumber * slices) + (sliceNumber % slices)));
                indexData.push_back(
                        static_cast<unsigned short &&>(((stackNumber + 1) * slices) + (sliceNumber % slices)));
            }
        }

        VertexBuffer vertexBuffer(&geometryData[0], static_cast<unsigned int>(7 * geometryData.size() * sizeof(float)));

        vertexArray = new GlVertexBinding();

        VertexAttributeDescription layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        vertexArray->addBuffer(&vertexBuffer, &layout);

        indexBuffer = new IndexBuffer(&indexData[0], static_cast<unsigned int>(indexData.size()));

        vertexArray->unbind();
        vertexBuffer.unbind();
        indexBuffer->unbind();
    }

    void StackedSphere::render(Shader &shader) const {
        Renderer::draw(*vertexArray, shader, *indexBuffer);
    }
}
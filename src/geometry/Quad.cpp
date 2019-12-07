//
// Created by artyomd on 1/2/19.
//

#include "Quad.h"
#include "api/Renderer.h"

namespace geometry {
    Quad::Quad(Point &point0, Point &point1, Point &point2, Point &point3) {
        float positions[] = {
                point0.x, point0.y, point0.z, point0.r, point0.g, point0.b, point0.a,
                point1.x, point1.y, point1.z, point1.r, point1.g, point1.b, point1.a,
                point2.x, point2.y, point2.z, point2.r, point2.g, point2.b, point2.a,
                point3.x, point3.y, point3.z, point3.r, point3.g, point3.b, point3.a
        };

        unsigned int indices[] = {
                0, 1, 2, 3
        };

        VertexBuffer vertexBuffer(positions, 4 * 7 * sizeof(float));

        vertexArray = new VertexArray();

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(4);
        vertexArray->addBuffer(&vertexBuffer, &layout);

        indexBuffer = new IndexBuffer(indices, 4);

        vertexArray->unbind();
        vertexBuffer.unbind();
        indexBuffer->unbind();
    }

    void Quad::render(Shader &shader) const {
        Renderer::draw(*vertexArray, shader, *indexBuffer);
    }
}
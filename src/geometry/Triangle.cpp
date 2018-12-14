//
// Created by Artyom Dangizyan on 2018-12-03.
//

#include "Triangle.h"
#include "Renderer.h"

Triangle::Triangle(Point &point0, Point &point1, Point &point2) {
    float positions[] = {
            point0.x, point0.y, point0.z, point0.r, point0.g, point0.b, point0.a,
            point1.x, point1.y, point1.z, point1.r, point1.g, point1.b, point1.a,
            point2.x, point2.y, point2.z, point2.r, point2.g, point2.b, point2.a
    };

    unsigned int indices[] = {
            0, 1, 2
    };

    VertexBuffer vertexBuffer(positions, 3 * 7 * sizeof(float));

    m_vertexArray = new VertexArray();

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(4);
    m_vertexArray->addBuffer(&vertexBuffer, &layout);

    m_indexBuffer = new IndexBuffer(indices, 3);

    m_vertexArray->unbind();
    vertexBuffer.unbind();
    m_indexBuffer->unbind();
}

void Triangle::render(Shader &shader) {
    Renderer::draw(*m_vertexArray, shader, *m_indexBuffer);
}

Triangle::~Triangle() {
    delete m_vertexArray;
    delete m_indexBuffer;
}
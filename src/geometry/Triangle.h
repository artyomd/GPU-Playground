//
// Created by Artyom Dangizyan on 2018-12-03.
//
#pragma once


#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Point.h"
#include "Shader.h"

class Triangle {
public:
    explicit Triangle(Point &point0, Point &point1, Point &point2);

    ~ Triangle();

    void render(Shader &shader);
private:
    VertexArray *m_vertexArray;
    IndexBuffer *m_indexBuffer;
};

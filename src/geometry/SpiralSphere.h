//
// Created by Artyom Dangizyan on 2018-12-05.
//

#pragma once

#include <vector>
#include "Point.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class SpiralSphere {

public:

    explicit SpiralSphere(float radius = 1.0f, unsigned int loops = 8, unsigned int segmentsPerLoop = 16);

    void render(Shader &shader) const;

private:
    VertexArray *m_vertexArray;
    IndexBuffer *m_indexBuffer;
    std::vector<Point> m_geometryData;
    std::vector<unsigned int> m_indexData;
};

//
// Created by Artyom Dangizyan on 2018-12-05.
//

#pragma once

#include <vector>
#include "Point.hpp"
#include "api/VertexArray.h"
#include "api/IndexBuffer.h"
#include "api/Shader.h"
#include "GeometryItem.h"

namespace geometry {
    class SpiralSphere : public GeometryItem {

    public:

        explicit SpiralSphere(float radius = 1.0f, unsigned int loops = 8, unsigned int segmentsPerLoop = 16);

        void render(Shader &shader) const override;

    private:
        std::vector<Point> geometryData;
        std::vector<unsigned int> indexData;
    };
}
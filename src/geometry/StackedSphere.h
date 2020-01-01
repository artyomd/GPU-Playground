//
// Created by Artyom Dangizyan on 2018-12-05.
//
#pragma once

#include <vector>
#include "api/gl/GlVertexBinding.h"
#include "api/IndexBuffer.h"
#include "api/Shader.h"
#include "Point.hpp"
#include "GeometryItem.h"

namespace geometry {
    class StackedSphere : public GeometryItem {

    public:
        explicit StackedSphere(float radius = 1.0f, unsigned int stacks = 8, unsigned int slices = 16);

        void render(Shader &shader) const override;

    private:
        std::vector<Point> geometryData;
        std::vector<unsigned int> indexData;
    };
}
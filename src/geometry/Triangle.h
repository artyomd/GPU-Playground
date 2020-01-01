//
// Created by Artyom Dangizyan on 2018-12-03.
//
#pragma once

#include "api/IndexBuffer.h"
#include "api/gl/GlVertexBinding.h"
#include "Point.hpp"
#include "api/Shader.h"
#include "GeometryItem.h"

namespace geometry {
    class Triangle : public GeometryItem {
    public:
        explicit Triangle(Point &point0, Point &point1, Point &point2);

        void render(Shader &shader) const override;
    };
}
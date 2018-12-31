//
// Created by Artyom Dangizyan on 2018-12-03.
//
#pragma once


#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Point.h"
#include "Shader.h"
#include "GeometryItem.h"

class Triangle : public GeometryItem {
public:
    explicit Triangle(Point &point0, Point &point1, Point &point2);

    void render(Shader &shader) const override;
};

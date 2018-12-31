//
// Created by Artyom Dangizyan on 2018-12-05.
//
#pragma once

#include <vector>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Point.h"
#include "GeometryItem.h"

class StackedSphere : public GeometryItem {

public:
    explicit StackedSphere(float radius = 1.0f, unsigned int stacks = 8, unsigned int slices = 16);

    void render(Shader &shader) const override;

private:
    std::vector<Point> geometryData;
    std::vector<unsigned int> indexData;
};
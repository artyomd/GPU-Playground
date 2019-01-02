//
// Created by artyomd on 1/2/19.
//

#pragma once

#include "GeometryItem.h"
#include "Point.h"

class Quad : public GeometryItem {
public:
    explicit Quad(Point &point0, Point &point1, Point &point2, Point &point3);

    void render(Shader &shader) const override;
};

//
// Created by Artyom Dangizyan on 2018-12-03.
//
#pragma once

#include <vector>

struct Point {
    float x, y, z; //position
    float a, r, g, b;//color

    Point operator+(const Point &p) {
        Point point{};
        point.x = p.x + this->x;
        point.y = p.y + this->y;
        point.z = p.z + this->z;
        point.a = p.a + this->a;
        point.r = p.r + this->r;
        point.g = p.g + this->g;
        point.b = p.b + this->b;
        return point;
    }

    Point operator/(const int &number) {
        Point point{};
        point.x = this->x / number;
        point.y = this->y / number;
        point.z = this->z / number;
        point.a = this->a / number;
        point.r = this->r / number;
        point.g = this->g / number;
        point.b = this->b / number;
        return point;
    }
};

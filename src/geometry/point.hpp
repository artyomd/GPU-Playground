//
// Created by Artyom Dangizyan on 2018-12-03.
//
#pragma once

namespace geometry {
struct Point {
  float x, y, z;
  float r, g, b, a;

  Point operator+(const Point &p) const {
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

  Point operator/(const int &number) const {
    Point point{};
    point.x = this->x / (float) number;
    point.y = this->y / (float) number;
    point.z = this->z / (float) number;
    point.a = this->a / (float) number;
    point.r = this->r / (float) number;
    point.g = this->g / (float) number;
    point.b = this->b / (float) number;
    return point;
  }
};
}
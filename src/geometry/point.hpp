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

  Point operator/(const float &number) const {
    Point point{};
    point.x = x / number;
    point.y = y / number;
    point.z = z / number;
    point.a = a / number;
    point.r = r / number;
    point.g = g / number;
    point.b = b / number;
    return point;
  }
};
}  // namespace geometry

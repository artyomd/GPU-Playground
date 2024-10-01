#pragma once

#include "color.hpp"

namespace geometry {
struct Point {
  float x, y, z;
  Point operator+(const Point &other) const {
    Point point{};
    point.x = other.x + this->x;
    point.y = other.y + this->y;
    point.z = other.z + this->z;
    return point;
  }

  Point operator/(const float &number) const {
    Point point{};
    point.x = x / number;
    point.y = y / number;
    point.z = z / number;
    return point;
  }
};
struct PointWithColor {
  Point point;
  Color color;
  PointWithColor(const float x, const float y, const float z, const uint8_t r, const uint8_t g, const uint8_t b,
                 const uint8_t a)
      : point{.x = x, .y = y, .z = z}, color{.r = r, .g = g, .b = b, .a = a} {}
};
}  // namespace geometry

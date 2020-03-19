//
// Created by Artyom Dangizyan on 2018-12-03.
//
#pragma once

namespace geometry {
struct Point {
  float x_, y_, z_;
  float a_, r_, g_, b_;

  Point operator+(const Point &p) {
    Point point{};
    point.x_ = p.x_ + this->x_;
    point.y_ = p.y_ + this->y_;
    point.z_ = p.z_ + this->z_;
    point.a_ = p.a_ + this->a_;
    point.r_ = p.r_ + this->r_;
    point.g_ = p.g_ + this->g_;
    point.b_ = p.b_ + this->b_;
    return point;
  }

  Point operator/(const int &number) {
    Point point{};
    point.x_ = this->x_/(float) number;
    point.y_ = this->y_/(float) number;
    point.z_ = this->z_/(float) number;
    point.a_ = this->a_/(float) number;
    point.r_ = this->r_/(float) number;
    point.g_ = this->g_/(float) number;
    point.b_ = this->b_/(float) number;
    return point;
  }
};
}
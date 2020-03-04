//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "Renderer.hpp"

namespace test {
class Test {
 protected:
  api::Renderer *renderer_;
 public:
  explicit Test(api::Renderer *renderer) : renderer_(renderer) {};

  virtual ~Test() = default;

  virtual void OnClear() {}

  virtual void OnUpdate(float delta_time) {};

  virtual void OnRender() {};

  virtual void OnImGuiRender() {};

  virtual void OnWindowSizeChanged(int width, int height) {};
};
}

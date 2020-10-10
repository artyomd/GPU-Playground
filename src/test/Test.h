//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include <utility>

#include "src/api/Renderer.hpp"

namespace test {
class Test {
 protected:
  std::shared_ptr<api::Renderer> renderer_;
 public:
  explicit Test(std::shared_ptr<api::Renderer> renderer) : renderer_(std::move(renderer)) {};

  virtual void OnClear() {}

  virtual void OnUpdate(float delta_time) {};

  virtual void OnRender() {};

  virtual void OnImGuiRender() {};

  virtual void OnViewportChange() {};

  virtual ~Test() = default;
};
}

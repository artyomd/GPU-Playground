//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include <utility>

#include "src/api/rendering_context.hpp"

namespace test {
class Test {
 protected:
  std::shared_ptr<api::RenderingContext> rendering_context_;
  int size_[2] = {0, 0};

  explicit Test(std::shared_ptr<api::RenderingContext> rendering_context) :
      rendering_context_(std::move(rendering_context)) {}

 public:
  virtual void OnUpdate(float delta_time) {};

  virtual void OnRender() {};

  virtual void OnImGuiRender() {};

  virtual void OnViewportChange(size_t width, size_t height) {
    size_[0] = width;
    size_[1] = height;
  };

  virtual ~Test() = default;
};
}

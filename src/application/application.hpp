//
// Created by artyomd on 12/6/19
//

#pragma once

#include <memory>
#include <utility>

#include "src/api/rendering_context.hpp"

namespace application {
class Application {
 protected:
  std::shared_ptr<api::RenderingContext> context_;

  int window_width_ = 640;
  int window_height_ = 480;

  explicit Application(std::shared_ptr<api::RenderingContext> context) : context_(std::move(context)) {}

 public:
  virtual void InitWindow() = 0;

  virtual void InitContext() = 0;

  virtual void InitImGui() = 0;

  virtual void Run() = 0;

  virtual void DestroyImGui() = 0;

  virtual void DestroyContext() = 0;

  virtual void DestroyWindow() = 0;
};
}

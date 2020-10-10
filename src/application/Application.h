//
// Created by artyomd on 12/6/19
//

#pragma once

#include <memory>

#include "src/api/Renderer.hpp"

namespace application {
class Application {
 protected:
  std::shared_ptr<api::Renderer> renderer_ = std::make_shared<api::Renderer>();
 public:
  Application() = default;
  virtual void InitWindow() = 0;

  virtual void InitContext() = 0;

  virtual void InitImGui() = 0;

  virtual void Run() = 0;

  virtual void DestroyImGui() = 0;

  virtual void DestroyContext() = 0;

  virtual void DestroyWindow() = 0;
};
}
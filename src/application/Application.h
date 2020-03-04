//
// Created by artyomd on 12/6/19.
//

#pragma once

namespace application {
class Application {
 protected:
  api::Renderer *renderer_ = nullptr;
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
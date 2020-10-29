//
// Created by artyomd on 12/6/19.
//

#pragma once

#include <GLFW/glfw3.h>

#include "src/application/test_application.hpp"

namespace application {
class GlfwApplication : public TestApplication {
 private:
  const float delta_time_ = 0.03333333333f;
 protected:
  GLFWwindow *window_ = nullptr;

  explicit GlfwApplication(const std::shared_ptr<api::RenderingContext> &context);

  virtual void SetupWindowHints() = 0;

  virtual void OnWindowSizeChanged() {};

  virtual bool PrepareFrame() = 0;

  virtual void CreateImGuiFrame() = 0;

  virtual void RenderImGui() = 0;

  virtual void DrawFrame() = 0;

  virtual void PrepareForShutdown() = 0;

 public:
  void InitWindow() final;

  void Run() final;

  void DestroyWindow() final;
};
}

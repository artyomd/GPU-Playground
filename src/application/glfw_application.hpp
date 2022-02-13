//
// Created by artyomd on 12/6/19.
//

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "src/application/test_application.hpp"

namespace application {
class GlfwApplication : public TestApplication {
 private:
  int window_width_ = 640;
  int window_height_ = 480;

  void InitWindow();

  void DestroyWindow();

 protected:
  GLFWwindow *window_ = nullptr;

  GlfwApplication();

  void InitContext() override;

  virtual void SetupWindowHints() = 0;

  virtual void OnWindowSizeChanged([[maybe_unused]] int width,
                                   [[maybe_unused]] int height){};

  void OnTestChanged(std::shared_ptr<test::Test> current_test) override;

  virtual bool PrepareFrame() = 0;

  virtual void RenderFrame() = 0;

  virtual void PrepareImGuiFrame() = 0;

  virtual void RenderImGui() = 0;

  void DestroyContext() override;

  void Loop() final;
};
}  // namespace application

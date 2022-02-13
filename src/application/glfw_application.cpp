//
// Created by artyomd on 12/6/19.
//

#include "src/application/glfw_application.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

namespace {
const float kDeltaTime = 0.03333333333f;
}

application::GlfwApplication::GlfwApplication() = default;

void application::GlfwApplication::InitContext() { InitWindow(); }

void application::GlfwApplication::InitWindow() {
  glfwSetErrorCallback([](int error, const char *description) {
    throw std::runtime_error("glfw error " + std::to_string(error) + ": " +
                             description);
  });

  if (glfwInit() != GLFW_TRUE) {
    throw std::runtime_error("can not init glfw");
  }

  SetupWindowHints();
  glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
  window_ = glfwCreateWindow(static_cast<int>(window_width_),
                             static_cast<int>(window_height_), "GPU Playground",
                             nullptr, nullptr);
  if (window_ == nullptr) {
    getchar();
    glfwTerminate();
    throw std::runtime_error("could not create window");
  }
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width,
                                             int height) {
    auto app =
        reinterpret_cast<GlfwApplication *>(glfwGetWindowUserPointer(window));
    app->window_width_ = width;
    app->window_height_ = height;
    app->OnWindowSizeChanged(width, height);
    app->GetCurrentTest()->OnViewportChange(static_cast<uint32_t>(width),
                                            static_cast<uint32_t>(height));
  });
}

void application::GlfwApplication::Loop() {
  while (glfwWindowShouldClose(window_) == 0) {
    glfwPollEvents();

    if (!PrepareFrame()) {
      continue;
    }

    auto current_test = GetCurrentTest();
    current_test->OnUpdate(kDeltaTime);
    current_test->OnRender();

    PrepareImGuiFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Test");
    current_test->OnImGuiRender();
    RenderMenu();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    RenderImGui();

    RenderFrame();
  }
  GetContext()->WaitForGpuIdle();
  CleanupTestMenu();
}

void application::GlfwApplication::DestroyWindow() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void application::GlfwApplication::DestroyContext() { DestroyWindow(); }

void application::GlfwApplication::OnTestChanged(
    std::shared_ptr<test::Test> current_test) {
  current_test->OnViewportChange(static_cast<uint32_t>(this->window_width_),
                                 static_cast<uint32_t>(this->window_height_));
}

//
// Created by artyomd on 12/6/19.
//

#include "src/application/glfw_application.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

application::GlfwApplication::GlfwApplication(const std::shared_ptr<api::RenderingContext> &context) : TestApplication(
    context) {}

void application::GlfwApplication::InitWindow() {
  glfwSetErrorCallback([](int error, const char *description) {
    throw std::runtime_error("glfw error " + std::to_string(error) + ": " + description);
  });

  if (!glfwInit()) {
    throw std::runtime_error("can not init glfw");
  }

  SetupWindowHints();

  window_ = glfwCreateWindow(window_width_, window_height_, "GPU Playground", nullptr, nullptr);
  if (window_ == nullptr) {
    getchar();
    glfwTerminate();
    throw std::runtime_error("could not create window");
  }
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<GlfwApplication *>(glfwGetWindowUserPointer(window));
    app->window_height_ = height;
    app->window_width_ = width;
    app->OnWindowSizeChanged();
    app->current_test_->OnViewportChange(width, height);
  });
}

void application::GlfwApplication::Run() {
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    if (!PrepareFrame()) {
      continue;
    }
    current_test_->OnUpdate(delta_time_);
    current_test_->OnRender();

    CreateImGuiFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Test");
    current_test_->OnImGuiRender();
    RenderMenu();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    RenderImGui();

    DrawFrame();
    PostRender();
  }
  PostRender();
  PrepareForShutdown();
}

void application::GlfwApplication::DestroyWindow() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}
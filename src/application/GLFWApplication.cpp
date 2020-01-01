//
// Created by artyomd on 12/6/19.
//

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "GLFWApplication.h"

namespace application {
    GLFWApplication::GLFWApplication() {
    }

    void GLFWApplication::initWindow() {
        glfwSetErrorCallback([](int error, const char *description) {
            throw std::runtime_error("Glfw Error " + std::to_string(error) + ": " + description);
        });

        if (!glfwInit()) {
            throw std::runtime_error("can not init glfw");
        }

        setupWindowHints();

        window = glfwCreateWindow(windowWidth, windowHeight, "GPU Playground", nullptr, nullptr);
        if (window == nullptr) {
            getchar();
            glfwTerminate();
            throw std::runtime_error("could not create widnow");
        }
        currentTest->onWindowSizeChanged(windowWidth, windowHeight);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window_, int width, int height) {
            auto app = reinterpret_cast<GLFWApplication *>(glfwGetWindowUserPointer(window_));
            app->windowHeight = height;
            app->windowWidth = width;
            app->onWindowSizeChanged();
            app->currentTest->onWindowSizeChanged(width, height);
        });
    }

    void GLFWApplication::run() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            currentTest->onClear(renderer);
            if (!prepareFrame()) {
                continue;
            }
            currentTest->onUpdate(deltaTime);
            currentTest->onRender();
            createImGuiFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Test");
            currentTest->onImGuiRender();
            renderMenu();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
            ImGui::Render();
            renderImGui();
            drawFrame();
        }
        prepareForShutdown();
    }


    void GLFWApplication::destroyWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    GLFWApplication::~GLFWApplication() {
    }
}
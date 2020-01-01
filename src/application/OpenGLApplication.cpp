//
// Created by artyomd on 12/6/19.
//

#include <GL/glew.h>
#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_impl_opengl3.h>
#include <vendor/imgui/imgui_impl_glfw.h>
#include "OpenGLApplication.h"
#include "api/Renderer.h"

namespace application {
    void OpenGLApplication::setupWindowHints() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void OpenGLApplication::onWindowSizeChanged() {
        GLCall(glViewport(0, 0, this->windowWidth, this->windowHeight));
    }

    void OpenGLApplication::initContext() {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        if (glewInit() != GLEW_OK) {
            getchar();
            glfwTerminate();
            throw std::runtime_error("can not init glew");
        }
        GLCall(glEnable(GL_CULL_FACE));
    }

    void OpenGLApplication::initImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");
        ImGui::StyleColorsDark();
    }

    bool OpenGLApplication::prepareFrame() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        float *color = renderer->getColor();
        GLCall(glClearColor(color[0], color[1], color[2], color[3]));
        return true;
    }

    void OpenGLApplication::createImGuiFrame() {
        ImGui_ImplOpenGL3_NewFrame();
    }

    void OpenGLApplication::renderImGui() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OpenGLApplication::drawFrame() {
        glfwSwapBuffers(window);
    }

    void OpenGLApplication::prepareForShutdown() {
        glFinish();
    }

    void OpenGLApplication::destroyImGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void OpenGLApplication::destroyContext() {
        glfwMakeContextCurrent(nullptr);
    }
}

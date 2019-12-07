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
        attachContext();
        GLCall(glViewport(0, 0, this->windowWidth, this->windowHeight));
        deAttachContext();
    }

    void OpenGLApplication::initContext() {
        attachContext();
        glfwSwapInterval(1);
        if (glewInit() != GLEW_OK) {
            getchar();
            glfwTerminate();
            throw std::runtime_error("can not init glew");
        }
        GLCall(glEnable(GL_CULL_FACE));
        deAttachContext();
    }

    void OpenGLApplication::initImGui() {
        attachContext();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");
        ImGui::StyleColorsDark();
        deAttachContext();
    }


    void OpenGLApplication::attachContext() {
        glfwMakeContextCurrent(window);
    }

//////////------ context is already attached
    void OpenGLApplication::clear() {
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void OpenGLApplication::createImGuiFrame() {
        ImGui_ImplOpenGL3_NewFrame();
    }

    void OpenGLApplication::renderImGui() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OpenGLApplication::swapBuffers() {
        glfwSwapBuffers(window);
    }

//////////------ context is already attached

    void OpenGLApplication::deAttachContext() {
        glfwMakeContextCurrent(nullptr);
    }

    void OpenGLApplication::destroyImGui() {
        attachContext();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        deAttachContext();
    }

    void OpenGLApplication::destroyContext() {
        //do nothing context is being held by glfw
    }
}

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <test/TestStarNestShader.hpp>
#include <test/TestSeascapeShader.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"
#include "TestMenu.h"
#include "TestShapingFunctionShader.hpp"
#include "TestClearColor.h"
#include "TestColorShader.hpp"
#include "TestSquaresShader.h"
#include "TestTriangle.h"
#include "TestSphere.h"
#include "TestTexture2d.h"

std::unordered_map<GLFWwindow *, std::function<void(int, int)>> callbacks;
const float deltaTime = 0.03333333333f;

void setWindowSizeCallback(GLFWwindow *window, std::function<void(int, int)> callback) {
    callbacks[window] = std::move(callback);
    glfwSetWindowSizeCallback(window, [](GLFWwindow *wnd, int w, int h) { callbacks[wnd](w, h); });
}

int main() {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "GPU Playground", nullptr, nullptr);
    if (window == nullptr) {
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        getchar();
        glfwTerminate();
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_CULL_FACE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    ImGui::StyleColorsDark();

    test::Test *currentTest = nullptr;
    auto *testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;

    setWindowSizeCallback(window, [&currentTest, &testMenu](int width, int height) {
        currentTest->onWindowSizeChanged(width, height);
        if (currentTest != testMenu) {
            testMenu->onWindowSizeChanged(width, height);
        }
    });

    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        testMenu->onWindowSizeChanged(width, height);
    }

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        GLCall(glViewport(0, 0, width, height));
    });

    testMenu->RegisterTest<test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<test::TestSeascapeShader>("Seascape");
    testMenu->RegisterTest<test::TestStarNestShader>("Star Nest");
    testMenu->RegisterTest<test::TestShapingFunctionShader>("Shader Shaping Function");
    testMenu->RegisterTest<test::TestColorShader>("Shader Colors");
    testMenu->RegisterTest<test::TestSquaresShader>("Shader Squares");
    testMenu->RegisterTest<test::TestTriangle>("Triangle");
    testMenu->RegisterTest<test::TestTexture2d>("Texture2D");
    testMenu->RegisterTest<test::TestSphere>("Sphere");

    while (!glfwWindowShouldClose(window)) {
        Renderer::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::clear();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        currentTest->onUpdate(deltaTime);
        currentTest->onRender();
        ImGui::Begin("Test");
        currentTest->onImGuiRender();
        if (currentTest != testMenu && ImGui::Button("<--")) {
            delete currentTest;
            currentTest = testMenu;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete currentTest;
    if (currentTest != testMenu) {
        delete testMenu;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
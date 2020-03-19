//
// Created by artyomd on 12/6/19.
//

#include <GL/glew.h>
#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_impl_opengl3.h>
#include <vendor/imgui/imgui_impl_glfw.h>
#include <api/gl/GlUtils.h>
#include <api/gl/GlRenderingContext.h>
#include "OpenGLApplication.h"

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *user_param) {
  fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type==GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
          type, severity, message);
}

namespace application {
void OpenGLApplication::SetupWindowHints() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void OpenGLApplication::InitContext() {
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);
  if (glewInit()!=GLEW_OK) {
    getchar();
    glfwTerminate();
    throw std::runtime_error("can not init glew");
  }
  GL_CALL(glEnable(GL_DEBUG_OUTPUT));
  GL_CALL(glDebugMessageCallback(MessageCallback, nullptr));
  //GL_CALL(glEnable(GL_CULL_FACE));
  context_ = new api::GlRenderingContext();
  renderer_->SetContext(context_);
  PrepareTestMenu();
}

void OpenGLApplication::InitImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 450");
  ImGui::StyleColorsDark();
}

bool OpenGLApplication::PrepareFrame() {
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
  float *color = renderer_->GetColor();
  GL_CALL(glClearColor(color[0], color[1], color[2], color[3]));
  return true;
}

void OpenGLApplication::CreateImGuiFrame() {
  ImGui_ImplOpenGL3_NewFrame();
}

void OpenGLApplication::RenderImGui() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLApplication::DrawFrame() {
  glfwSwapBuffers(window_);
}

void OpenGLApplication::PrepareForShutdown() {
  glFinish();
}

void OpenGLApplication::DestroyImGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void OpenGLApplication::DestroyContext() {
  DeleteTestMenu();
  delete context_;
  glfwMakeContextCurrent(nullptr);
}
}

//
// Created by artyomd on 12/6/19.
//

#include <GL/glew.h>
#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_impl_opengl3.h>
#include <vendor/imgui/imgui_impl_glfw.h>
#include <api/gl/GlUtils.h>
#include <api/gl/GlRenderingContext.h>
#include "OpenGlApplication.h"

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *user_param) {
  std::string msg_source;
  switch (source) {
    case GL_DEBUG_SOURCE_API:msg_source = "WINDOW_SYSTEM";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:msg_source = "SHADER_COMPILER";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:msg_source = "THIRD_PARTY";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:msg_source = "APPLICATION";
      break;
    case GL_DEBUG_SOURCE_OTHER:msg_source = "OTHER";
      break;
    default:msg_source = "unknown source";
      break;
  }

  std::string msg_type;
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:msg_type = "ERROR";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:msg_type = "DEPRECATED_BEHAVIOR";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:msg_type = "UNDEFINED_BEHAVIOR";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:msg_type = "PORTABILITY";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:msg_type = "PERFORMANCE";
      break;
    case GL_DEBUG_TYPE_OTHER:msg_type = "OTHER";
      break;
    default:msg_type = "unknown type";
      break;
  }

  std::string msg_severity;
  switch (severity) {
    case GL_DEBUG_SEVERITY_LOW:msg_severity = "LOW";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:msg_severity = "MEDIUM";
      break;
    case GL_DEBUG_SEVERITY_HIGH:msg_severity = "HIGH";
      break;
    default:msg_severity = "unknown severity";
      break;
  }

  printf("glDebugMessage:\n%s \n type = %s source = %s severity = %s\n",
         message,
         msg_type.c_str(),
         msg_source.c_str(),
         msg_severity.c_str());

}

namespace application {
void OpenGlApplication::SetupWindowHints() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DEPTH_BITS, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 1);
}

void OpenGlApplication::InitContext() {
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(0);
  if (glewInit()!=GLEW_OK) {
    getchar();
    glfwTerminate();
    throw std::runtime_error("can not init glew");
  }
  GL_CALL(glEnable(GL_DEBUG_OUTPUT));
  GL_CALL(glDebugMessageCallback(MessageCallback, nullptr));
  context_ = new api::GlRenderingContext();
  renderer_->SetContext(context_);
  PrepareTestMenu();
}

void OpenGlApplication::InitImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  ImGui::StyleColorsDark();
}

bool OpenGlApplication::PrepareFrame() {
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  float *color = renderer_->GetColor();
  GL_CALL(glClearColor(color[0], color[1], color[2], color[3]));
  return true;
}

void OpenGlApplication::CreateImGuiFrame() {
  ImGui_ImplOpenGL3_NewFrame();
}

void OpenGlApplication::RenderImGui() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGlApplication::DrawFrame() {
  glfwSwapBuffers(window_);
}

void OpenGlApplication::PrepareForShutdown() {
  glFinish();
}

void OpenGlApplication::DestroyImGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void OpenGlApplication::DestroyContext() {
  DeleteTestMenu();
  delete context_;
  glfwMakeContextCurrent(nullptr);
}
}

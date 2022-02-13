//
// Created by artyomd on 12/6/19.
//

#pragma once

#include "src/api/opengl/opengl_rendering_context.hpp"
#include "src/application/glfw_application.hpp"

namespace application {
class OpenGlApplication : public GlfwApplication {
 private:
  void InitImGui();
  static void DestroyImGui();

 protected:
  void SetupWindowHints() final;
  void InitContext() final;
  bool PrepareFrame() final;
  void PrepareImGuiFrame() final;
  void RenderImGui() final;
  void RenderFrame() final;
  void DestroyContext() override;

 public:
  OpenGlApplication();
};
}  // namespace application

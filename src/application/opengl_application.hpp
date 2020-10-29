//
// Created by artyomd on 12/6/19.
//

#pragma once

#include "src/application/glfw_application.hpp"
#include "src/api/opengl/opengl_rendering_context.hpp"

namespace application {
class OpenGlApplication : public GlfwApplication {
 protected:
  void SetupWindowHints() final;

  bool PrepareFrame() final;

  void CreateImGuiFrame() final;

  void RenderImGui() final;

  void DrawFrame() final;

  void PrepareForShutdown() final;
 public:
  OpenGlApplication();

  void InitContext() final;

  void InitImGui() final;

  void DestroyImGui() final;

  void DestroyContext() override;
};
}

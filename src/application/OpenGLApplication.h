//
// Created by artyomd on 12/6/19.
//

#pragma once

#include "GlfwApplication.h"
#include "api/gl/GlRenderingContext.h"

namespace application {
class OpenGLApplication : public GlfwApplication {
 private:
  api::GlRenderingContext *context_;
 protected:
  void SetupWindowHints() final;

  bool PrepareFrame() final;

  void CreateImGuiFrame() final;

  void RenderImGui() final;

  void DrawFrame() final;

  void PrepareForShutdown() final;

 public:
  void InitContext() final;

  void InitImGui() final;

  void DestroyImGui() final;

  void DestroyContext() override;
};
}
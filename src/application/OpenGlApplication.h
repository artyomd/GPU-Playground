//
// Created by artyomd on 12/6/19.
//

#pragma once

#include "src/application/GlfwApplication.h"
#include "src/api/gl/GlRenderingContext.h"

namespace application {
class OpenGlApplication : public GlfwApplication {
 private:
  std::shared_ptr<api::GlRenderingContext> context_ = std::make_shared<api::GlRenderingContext>();
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
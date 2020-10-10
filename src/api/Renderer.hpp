//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <utility>

#include "src/api/RenderingContext.h"
#include "src/api/RenderingPipeline.h"

namespace api {

class Renderer {
 private:
  Pixel_RGBA clear_color_ = {0.0, 0.0, 0.0, 1.0};
  std::shared_ptr<RenderingContext> context_ = nullptr;
  Size viewport_size_{};

 public:
  Renderer() = default;

  [[nodiscard]] const Pixel_RGBA &GetClearColor() const {
    return clear_color_;
  }
  void SetClearColor(const Pixel_RGBA& clear_color) {
    clear_color_ = clear_color;
  }

  void SetContext(std::shared_ptr<RenderingContext> context) {
    if (context_ != nullptr) {
      throw std::runtime_error("SetContext must only be called once");
    }
    context_ = std::move(context);
    context_->SetViewportSize(viewport_size_.width, viewport_size_.height);
  }

  void SetViewport(int width, int height) {
    viewport_size_ = {width, height};
    if (context_ != nullptr) {
      context_->SetViewportSize(viewport_size_.width, viewport_size_.height);
    }
  }

  [[nodiscard]] const Size &GetViewportSize() const {
    return viewport_size_;
  }

  [[nodiscard]] const std::shared_ptr<RenderingContext> &GetContext() const {
    return context_;
  }

  static void Render(const std::shared_ptr<RenderingPipeline> &pipeline) {
    pipeline->Render();
  }
};
}

//
// Created by artyomd on 12/30/19.
//

#pragma once

#include "src/api/RenderingContext.h"
#include "src/api/RenderingPipeline.h"

namespace api {
struct Size {
  int width_ = 0;
  int height_ = 0;
};

class Renderer {
 private:
  float *clear_color_ = new float[4]{0.0f, 0.0f, 0.0f, 0.0f};
  RenderingContext *context_ = nullptr;
  Size viewport_size_{};

 public:
  Renderer() = default;

  ~Renderer() {
    delete[] clear_color_;
  }

  void SetClearColor(float red, float green, float blue, float alpha) {
    this->clear_color_[0] = red;
    this->clear_color_[1] = green;
    this->clear_color_[2] = blue;
    this->clear_color_[3] = alpha;
  }

  float *GetColor() {
    return clear_color_;
  }

  void SetContext(RenderingContext *context) {
    if (context_ != nullptr) {
      throw std::runtime_error("SetContext must only be called once");
    }
    context_ = context;
    context_->SetViewportSize(viewport_size_.width_, viewport_size_.height_);
  }

  void SetViewport(int width, int height) {
    viewport_size_ = {width, height};
    if (context_ != nullptr) {
      context_->SetViewportSize(viewport_size_.width_, viewport_size_.height_);
    }
  }

  [[nodiscard]] const Size &GetViewportSize() const {
    return viewport_size_;
  }

  RenderingContext *GetRenderingContext() {
    return context_;
  }

  static void Render(RenderingPipeline *pipeline) {
    pipeline->Render();
  }
};
}

//
// Created by artyomd on 12/30/19.
//

#pragma once

#include "RenderingContext.h"
#include "RenderingPipeline.h"

namespace api {
class Renderer {
 private:
  float *clear_color_ = new float[4]{0.0f, 0.0f, 0.0f, 0.0f};
  RenderingContext *context_ = nullptr;
  int viewport_height_ = 0;
  int viewport_width_ = 0;

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
    if (context_!=nullptr) {
      throw std::runtime_error("SetContext must only be called once");
    }
    context_ = context;
    context->SetViewportSize(viewport_width_, viewport_height_);
  }

  void SetViewport(int width, int height) {
    this->viewport_width_ = width;
    this->viewport_height_ = height;
    if (context_!=nullptr) {
      context_->SetViewportSize(viewport_width_, viewport_height_);
    }
  }

  RenderingContext *GetRenderingContext() {
    return context_;
  }

  static void Render(RenderingPipeline *pipeline) {
    pipeline->Render();
  }
};
}

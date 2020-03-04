//
// Created by artyomd on 12/30/19.
//

#pragma once

#include "RenderingContext.h"
#include "RenderingPipeline.h"

namespace api {
class Renderer {
 private:
  float *clear_color_;
  RenderingContext *context_ = nullptr;

 public:
  explicit Renderer(RenderingContext *context) : context_(context) {
    clear_color_ = new float[4]{0.0f, 0.0f, 0.0f, 0.0f};
  }

  ~Renderer() {
    delete clear_color_;
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

  RenderingContext *GetRenderingContext() {
    return context_;
  }

  void Render(RenderingPipeline *pipeline) {
    pipeline->Render();
  }
};
}

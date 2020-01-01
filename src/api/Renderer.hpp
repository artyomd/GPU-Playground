//
// Created by artyomd on 12/30/19.
//

#pragma once

#include "RenderingContext.h"
#include "Shader.h"
#include "RenderingPipline.h"

namespace api {
    class Renderer {
    private:
        float *clearColor;
        RenderingContext *context = nullptr;

    public:
        explicit Renderer(RenderingContext *context) : context(context) {
            clearColor = new float[4]{0.0f, 0.0f, 0.0f, 0.0f};
        }

        ~Renderer() {
            delete clearColor;
        }

        void setClearColor(float red, float green, float blue, float alpha) {
            this->clearColor[0] = red;
            this->clearColor[1] = green;
            this->clearColor[2] = blue;
            this->clearColor[3] = alpha;
        }

        float *getColor() {
            return this->clearColor;
        }

        RenderingContext *getRenderingContext() {
            return context;
        }

        void render(RenderingPipeline *pipeline) {

        }
    };
}

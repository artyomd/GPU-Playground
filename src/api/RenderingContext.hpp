//
// Created by artyomd on 12/30/19.
//

#pragma once

class RenderingContext {
private:
    float *clearColor = new float[4]{0.0f, 0.0f, 0.0f, 0.0f};
public:
    void setClearColor(float red, float green, float blue, float alpha) {
        this->clearColor[0] = red;
        this->clearColor[1] = green;
        this->clearColor[2] = blue;
        this->clearColor[3] = alpha;
    }

    float *getColor() {
        return this->clearColor;
    }
};

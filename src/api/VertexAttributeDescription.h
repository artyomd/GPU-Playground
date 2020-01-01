//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include <vector>
#include <GL/glew.h>
#include <cassert>

struct VertexAttribute {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }
        assert(false);
        return 0;
    }
};

class VertexAttributeDescription {

private:
    std::vector<VertexAttribute> elements;
    unsigned int stride;
public:
    VertexAttributeDescription() : stride(0) {}

    template<typename T>
    void Push(unsigned int count) {
        assert(false); //Not implemented
    }

    inline const std::vector<VertexAttribute> getElements() const {
        return elements;
    }

    inline const unsigned int getStride() const {
        return stride;
    }
};

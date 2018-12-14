//
// Created by Artyom Dangizyan on 11/18/18.
//
#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "Test.h"
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

namespace test {
    class TestTexture2d : public Test {
    public:
        TestTexture2d();

        ~TestTexture2d() override;

        void onRender() override;

        void onImGuiRender() override;

    private:
        Shader *m_shader;
        VertexArray *m_vertexArray;
        IndexBuffer *m_indexBuffer;
        glm::mat4 projection;
        glm::vec3 translation;
        glm::vec3 scale;
        glm::vec3 rotate;
    };
}

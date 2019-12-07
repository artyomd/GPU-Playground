//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "TestTexture2d.h"
#include "api/Texture.h"
#include "api/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <api/ShaderPropertyMatrix4f.h>
#include <api/ShaderPropertyInt1.h>
#include "imgui/imgui.h"

namespace test {
    test::TestTexture2d::TestTexture2d() : TestMVP() {
        float positions[] = {
                -0.5f, -0.5f, 0.0f, 0.0f,
                0.5f, -0.5f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.0f, 1.0f,
                0.5f, 0.5f, 1.0f, 1.0f
        };

        unsigned int indices[] = {
                0, 1, 2, 3
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexBuffer vertexBuffer(positions, 4 * 4 * sizeof(float));

        vertexArray = new VertexArray();

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vertexArray->addBuffer(&vertexBuffer, &layout);

        indexBuffer = new IndexBuffer(indices, 6);

        shader = new Shader("../res/shader/texture2d_vertex.glsl", "../res/shader/texture2d_fragment.glsl");
        shader->bind();

        Texture *texture = new Texture("../res/textures/image.png");
        unsigned int slot = 0;
        texture->bind(slot);

        shader->setUniform("u_Texture", new ShaderPropertyInt1(slot));

        vertexArray->unbind();
        vertexBuffer.unbind();
        indexBuffer->unbind();
        shader->unbind();
    }

    void TestTexture2d::onRender() {
        shader->bind();
        glm::mat4 mvp = computeMVP();
        auto property = new ShaderPropertyMatrix4f(mvp);
        shader->setUniform("u_MVP", property);
        delete property;
        Renderer::draw(*vertexArray, *shader, *indexBuffer);
        shader->unbind();
    }

    TestTexture2d::~TestTexture2d() {
        delete shader;
        delete vertexArray;
        delete indexBuffer;
        GLCall(glDisable(GL_BLEND));
    }
}

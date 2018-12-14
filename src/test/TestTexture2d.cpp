//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "TestTexture2d.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"

namespace test {
    test::TestTexture2d::TestTexture2d() {
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

        m_vertexArray = new VertexArray();

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_vertexArray->addBuffer(&vertexBuffer, &layout);

        m_indexBuffer = new IndexBuffer(indices, 6);

        m_shader = new Shader("../res/shader/texture2d_vertex.glsl", "../res/shader/texture2d_fragment.glsl");
        m_shader->bind();

        projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f);
        translation = glm::vec3(0.0f, 0.0f, 0.0f);
        scale = glm::vec3(1.0f, 1.0f, 1.0f);
        rotate = glm::vec3(0.0f, 0.0f, 0.0f);

        Texture *texture = new Texture("../res/textures/image.png");
        unsigned int slot = 0;
        texture->bind(slot);

        m_shader->setUniform1i("u_Texture", slot);

        m_vertexArray->unbind();
        vertexBuffer.unbind();
        m_indexBuffer->unbind();
        m_shader->unbind();
    }

    void TestTexture2d::onImGuiRender() {
        ImGui::SliderFloat2("translation", &translation.x, -2.0f, 2.0f);
        ImGui::SliderFloat2("scale", &scale.x, 0, 20);
        ImGui::SliderFloat3("rotate", &rotate.x, 0, 360);
    }

    void TestTexture2d::onRender() {
        m_shader->bind();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 mvp = projection * model;
        m_shader->setUniformMatrix4f("u_MVP", mvp);
        Renderer::draw(*m_vertexArray, *m_shader, *m_indexBuffer);
    }

    TestTexture2d::~TestTexture2d() {
        delete m_shader;
        delete m_vertexArray;
        delete m_indexBuffer;
        GLCall(glDisable(GL_BLEND));
    }
}

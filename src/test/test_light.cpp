//
// Created by artyomd on 5/1/20.
//
#include "src/test/test_light.hpp"

#include "src/shaders/shaders.hpp"

#include <glm/gtx/hash.hpp>
#include <tinyobjloader/tiny_obj_loader.h>
#include <unordered_map>
#include <iostream>
#include <stb_image.h>

namespace {
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  bool operator==(const Vertex &other) const {
    return position == other.position && normal == other.normal;
  }
};
}
namespace std {
template<>
struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.position));
    glm::detail::hash_combine(seed, hash<glm::vec2>()(vertex.normal));
    return seed;
  }
};
}

test::TestLight::TestLight(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "../res/models/dragon.obj", "../res/models")) {
    throw std::runtime_error(warn + err);
  }
  std::cout << "warn:" << warn << std::endl;
  std::cout << "err:" << err << std::endl;
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::unordered_map<Vertex, uint32_t> unique_vertices = {};

  for (const auto &shape: shapes) {
    for (const auto &index: shape.mesh.indices) {
      Vertex vertex = {};
      vertex.position = {
          attrib.vertices[3 * static_cast<unsigned long>(index.vertex_index) + 0],
          attrib.vertices[3 * static_cast<unsigned long>(index.vertex_index) + 1],
          attrib.vertices[3 * static_cast<unsigned long>(index.vertex_index) + 2]
      };
      vertex.normal = {
          attrib.normals[3 * static_cast<unsigned long>(index.vertex_index) + 0],
          attrib.normals[3 * static_cast<unsigned long>(index.vertex_index) + 1],
          attrib.normals[3 * static_cast<unsigned long>(index.vertex_index) + 2]
      };
      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(unique_vertices[vertex]);
    }
  }

  api::VertexBufferLayout vertex_buffer_layout;
  vertex_buffer_layout.Push({0, api::DataType::FLOAT, 3});
  vertex_buffer_layout.Push({1, api::DataType::FLOAT, 3});
  auto
      vertex_buffer = rendering_context_->CreateVertexBuffer(6 * vertices.size() * sizeof(float), vertex_buffer_layout);
  vertex_buffer->Update(vertices.data());

  auto index_buffer =
      rendering_context_->CreateIndexBuffer(static_cast<uint32_t>(indices.size()), api::DataType::UINT_32);
  index_buffer->Update(indices.data());

  auto vertex_shader = rendering_context_->CreateShader(light_vertex,
                                                        "main",
                                                        api::ShaderType::VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(light_fragment,
                                                          "main",
                                                          api::ShaderType::FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer, index_buffer,
                                                         vertex_shader, fragment_shader,
                                                         {api::DrawMode::TRIANGLE_LIST,
                                                          api::CullMode::BACK,
                                                          api::FrontFace::CCW,
                                                          true,
                                                          api::CompareOp::LESS});
  buffer_.eye = glm::vec4(0.0, 8.0, 0.0, 0.0);
  buffer_.light = glm::vec4(3.0, 0.0, 1.0, 0.0);
  pipeline_->UpdateUniformBuffer(1, &buffer_);

}

void test::TestLight::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = perspective_projection_;
  ubo_.view = glm::lookAt(glm::vec3(0.0F, 8.0F, 0.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F));
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Render();
}

//
// Created by artyomd on 5/1/20.
//
#include "src/test/test_obj.hpp"

#include <glm/gtx/hash.hpp>
#include <tinyobjloader/tiny_obj_loader.h>
#include <unordered_map>

struct Vertex {
  glm::vec3 position;
  glm::vec2 tex_coord;
  bool operator==(const Vertex &other) const {
    return position == other.position && tex_coord == other.tex_coord;
  }
};

template<>
struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.position));
    glm::detail::hash_combine(seed, hash<glm::vec2>()(vertex.tex_coord));
    return seed;
  }
};

test::TestObj::TestObj(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "../res/models/chalet.obj")) {
    throw std::runtime_error(warn + err);
  }

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::unordered_map<Vertex, uint32_t> unique_vertices = {};

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex = {};
      vertex.position = {
          attrib.vertices[3 * index.vertex_index + 0],
          attrib.vertices[3 * index.vertex_index + 1],
          attrib.vertices[3 * index.vertex_index + 2]
      };
      vertex.tex_coord = {
          attrib.texcoords[2 * index.texcoord_index + 0],
          attrib.texcoords[2 * index.texcoord_index + 1]
      };
      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(unique_vertices[vertex]);
    }
  }

  api::VertexBufferLayout vertex_buffer_layout;
  vertex_buffer_layout.Push<float>(3);
  vertex_buffer_layout.Push<float>(2);
  vertex_buffer_ = rendering_context_->CreateVertexBuffer(5 * vertices.size() * sizeof(float), vertex_buffer_layout);
  vertex_buffer_->Update(vertices.data());

  index_buffer_ = rendering_context_->CreateIndexBuffer(indices.size(), api::DataType::DATA_TYPE_UINT_32);
  index_buffer_->Update(indices.data());

  vertex_shader_ = rendering_context_->CreateShader("../res/shader/compiled/texture2d_vertex.spv",
                                                    "main",
                                                    api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = rendering_context_->CreateShader("../res/shader/compiled/texture2d_fragment.spv",
                                                      "main",
                                                      api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer_, index_buffer_,
                                                         vertex_shader_, fragment_shader_,
                                                         {api::DrawMode::TRIANGLE_LIST,
                                                          api::CullMode::BACK,
                                                          api::FrontFace::CCW,
                                                          true,
                                                          api::CompareOp::LESS});

  uniform_buffer_ = rendering_context_->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                            0,
                                                            api::ShaderType::SHADER_TYPE_VERTEX);
  obj_texture_ = rendering_context_->CreateTexture2D("../res/textures/chalet.jpg",
                                                     1,
                                                     api::ShaderType::SHADER_TYPE_FRAGMENT);
  obj_texture_->SetSampler({api::Filter::LINEAR,
                            api::Filter::LINEAR,
                            api::AddressMode::CLAMP_TO_EDGE,
                            api::AddressMode::CLAMP_TO_EDGE,
                            api::AddressMode::CLAMP_TO_EDGE});
  pipeline_->AddUniform(uniform_buffer_);
  pipeline_->AddUniform(obj_texture_);
}

void test::TestObj::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = perspective_projection_;
  ubo_->view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform_buffer_->Update(ubo_.get());
  pipeline_->Render();
}
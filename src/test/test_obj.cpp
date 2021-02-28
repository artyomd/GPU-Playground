//
// Created by artyomd on 5/1/20.
//
#include "src/test/test_obj.hpp"

#include <glm/gtx/hash.hpp>
#include <tinyobjloader/tiny_obj_loader.h>
#include <unordered_map>
#include <iostream>
#include <stb_image.h>
#include <snowhouse/snowhouse.h>

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
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "../res/models/chalet.obj", "../res/models")) {
    throw std::runtime_error(warn + err);
  }
  std::cout << "warn:" << warn << std::endl;
  std::cout << "err:" << err << std::endl;
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
  size_t stride = sizeof(float) * 5;
  vertex_buffer_layout.Push({0, api::DataType::DATA_TYPE_FLOAT, 3});
  vertex_buffer_layout.Push({1, api::DataType::DATA_TYPE_FLOAT, 2});
  auto
      vertex_buffer = rendering_context_->CreateVertexBuffer(5 * vertices.size() * sizeof(float), vertex_buffer_layout);
  vertex_buffer->Update(vertices.data());

  auto index_buffer = rendering_context_->CreateIndexBuffer(indices.size(), api::DataType::DATA_TYPE_UINT_32);
  index_buffer->Update(indices.data());

  auto vertex_shader = rendering_context_->CreateShader({
#include SHADER(texture2d_vertex)
                                                        },
                                                        "main",
                                                        api::ShaderType::SHADER_TYPE_VERTEX);
  auto fragment_shader = rendering_context_->CreateShader({
#include SHADER(texture2d_fragment)
                                                          },
                                                          "main",
                                                          api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer, index_buffer,
                                                         vertex_shader, fragment_shader,
                                                         {api::DrawMode::TRIANGLE_LIST,
                                                          api::CullMode::BACK,
                                                          api::FrontFace::CCW,
                                                          true,
                                                          api::CompareOp::LESS});
  stbi_set_flip_vertically_on_load(true);
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load("../res/textures/chalet.jpg", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }
  auto obj_texture =
      rendering_context_->CreateTexture2D(tex_width, tex_height, api::PixelFormat::PIXEL_FORMAT_R8G8B8A8_SRGB);
  obj_texture->Load(pixels);
  stbi_image_free(pixels);
  obj_texture->SetSampler({api::Filter::LINEAR,
                           api::Filter::LINEAR,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE});
  pipeline_->SetTexture(1, obj_texture);
}

void test::TestObj::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = perspective_projection_;
  ubo_.view = glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F));
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Render();
}

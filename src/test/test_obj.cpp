//
// Created by artyomd on 5/1/20.
//
#include "src/test/test_obj.hpp"

#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>

#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "src/shaders/shaders.hpp"

namespace {
struct Vertex {
  glm::vec3 position;
  glm::vec2 tex_coord;
  bool operator==(const Vertex &other) const {
    return position == other.position && tex_coord == other.tex_coord;
  }
};
}  // namespace

namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.position));
    glm::detail::hash_combine(seed, hash<glm::vec2>()(vertex.tex_coord));
    return seed;
  }
};
}  // namespace std
test::TestObj::TestObj(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        "../res/models/chalet.obj", "../res/models")) {
    throw std::runtime_error(warn + err);
  }
  spdlog::warn(warn);
  spdlog::error(err);
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::unordered_map<Vertex, uint32_t> unique_vertices;

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex = {};
      vertex.position = {
          attrib
              .vertices[3 * static_cast<unsigned long>(index.vertex_index) + 0],
          attrib
              .vertices[3 * static_cast<unsigned long>(index.vertex_index) + 1],
          attrib.vertices[3 * static_cast<unsigned long>(index.vertex_index) +
                          2]};
      vertex.tex_coord = {
          attrib
              .texcoords[2 * static_cast<unsigned long>(index.texcoord_index) +
                         0],
          attrib
              .texcoords[2 * static_cast<unsigned long>(index.texcoord_index) +
                         1]};
      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(unique_vertices[vertex]);
    }
  }

  api::VertexBufferLayout vertex_buffer_layout;
  vertex_buffer_layout.Push({0, api::DataType::FLOAT, 3});
  vertex_buffer_layout.Push({1, api::DataType::FLOAT, 2});
  auto vertex_buffer = rendering_context_->CreateBuffer(
      5 * vertices.size() * sizeof(float), api::BufferUsage::VERTEX_BUFFER,
      api::MemoryType::DEVICE_LOCAL);
  vertex_buffer->Update(vertices.data());

  auto index_buffer = rendering_context_->CreateBuffer(
      indices.size() * sizeof(uint32_t), api::BufferUsage::INDEX_BUFFER,
      api::MemoryType::DEVICE_LOCAL);
  index_count_ = indices.size();
  index_buffer->Update(indices.data());

  auto vertex_shader = rendering_context_->CreateShader(
      texture2d_vertex, "main", api::ShaderType::VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(
      texture2d_fragment, "main", api::ShaderType::FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(
      vertex_shader, fragment_shader, vertex_buffer_layout,
      {api::DrawMode::TRIANGLE_LIST, api::CullMode::BACK, api::FrontFace::CCW,
       true, api::CompareOp::LESS});
  pipeline_->SetVertexBuffer(vertex_buffer);
  pipeline_->SetIndexBuffer(index_buffer, api::DataType::UINT_32);
  stbi_set_flip_vertically_on_load(true);
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load("../res/textures/chalet.jpg", &tex_width,
                              &tex_height, &tex_channels, STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }
  auto obj_texture = rendering_context_->CreateTexture2D(
      static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height),
      api::PixelFormat::RGBA_8_SRGB);
  obj_texture->Load(pixels);
  stbi_image_free(pixels);
  obj_texture->SetSampler({api::Filter::LINEAR, api::Filter::LINEAR,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE});
  pipeline_->SetTexture(1, obj_texture);
}

void test::TestObj::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = perspective_projection_;
  ubo_.view =
      glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 0.0F),
                  glm::vec3(0.0F, 0.0F, 1.0F));
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Draw(index_count_, 0);
}

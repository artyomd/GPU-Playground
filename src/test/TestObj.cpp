//
// Created by artyomd on 5/1/20.
//
#include <vendor/tinyobjloader/tiny_obj_loader.h>
#include <unordered_map>
#include "TestObj.h"
#include <glm/gtx/hash.hpp>
#include <GL/glew.h>

struct Vertex {
  glm::vec3 position_;
  glm::vec2 tex_coord_;
  bool operator==(const Vertex &other) const {
    return position_==other.position_ && tex_coord_==other.tex_coord_;
  }
};

namespace std {
template<>
struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.position_));
    glm::detail::hash_combine(seed, hash<glm::vec2>()(vertex.tex_coord_));
    return seed;
  }
};
}

test::TestObj::TestObj(api::Renderer *renderer) : TestModel(renderer) {
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
      vertex.position_ = {
          attrib.vertices[3*index.vertex_index + 0],
          attrib.vertices[3*index.vertex_index + 1],
          attrib.vertices[3*index.vertex_index + 2]
      };
      vertex.tex_coord_ = {
          attrib.texcoords[2*index.texcoord_index + 0],
          1.0f - attrib.texcoords[2*index.texcoord_index + 1]
      };
      if (unique_vertices.count(vertex)==0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(unique_vertices[vertex]);
    }
  }

  auto *context = renderer->GetRenderingContext();

  vertex_buffer_ = context->CreateVertexBuffer(vertices.data(), 5*vertices.size()*sizeof(float));
  vertex_buffer_layout_ = new api::VertexBufferLayout();
  vertex_buffer_layout_->Push<float>(3);
  vertex_buffer_layout_->Push<float>(2);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, vertex_buffer_layout_);
  index_buffer_ = context->CreateIndexBuffer(indices.data(), indices.size(), api::DATA_TYPE_UINT_32);

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  obj_texture_ = context->CreateTexture2D("../res/textures/chalet.jpg",
                                          1,
                                          api::ShaderType::SHADER_TYPE_FRAGMENT);
  std::vector<api::Uniform *> uniforms;
  uniforms.push_back(uniform_buffer_);
  uniforms.push_back(obj_texture_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  vertex_shader_ = context->CreateShader("../res/shader/compiled/texture2d_vertex.spv",
                                         "../res/shader/texture2d_vertex.glsl",
                                         "main",
                                         api::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/texture2d_fragment.spv",
                                           "../res/shader/texture2d_fragment.glsl",
                                           "main",
                                           api::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(vertex_binding_, index_buffer_,
                                              vertex_shader_, fragment_shader_, pipeline_layout_,
                                              {api::CullMode::BACK,
                                               api::FrontFace::CCW,
                                               true,
                                               api::DepthFunction::LESS});
}

void test::TestObj::OnClear() {
  renderer_->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void test::TestObj::OnRender() {
  ubo_->model_ = ComputeModelMatrix();
  ubo_->proj_ = renderer_->GetRenderingContext()->GetPrespectiveProjection();
  ubo_->view_ = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform_buffer_->Update(ubo_);
  api::Renderer::Render(pipeline_);
}

void test::TestObj::OnViewportChange() {
  pipeline_->ViewportChanged();
}

test::TestObj::~TestObj() {
  auto context = renderer_->GetRenderingContext();
  context->FreeGraphicsPipeline(pipeline_);
  context->FreeRenderingPipelineLayout(pipeline_layout_);
  context->DeleteShader(fragment_shader_);
  context->DeleteShader(vertex_shader_);
  context->DeleteUniformBuffer(uniform_buffer_);
  context->DeleteTexture2D(obj_texture_);
  context->FreeIndexBuffer(index_buffer_);
  context->FreeVertexBiding(vertex_binding_);
  delete (vertex_buffer_layout_);
  context->FreeVertexBuffer(vertex_buffer_);
}
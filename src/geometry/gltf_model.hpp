//
// Created by Artyom Dangizyan on 12/30/20.
//

#pragma once

#include <string>
#include <tinygltf/tiny_gltf.h>
#include <glm/ext.hpp>

#include "src/api/rendering_context.hpp"

namespace geometry {

struct Camera {
  glm::mat4 view = glm::mat4(1.0);
  glm::mat4 proj = glm::mat4(1.0);
  std::string name;
};

struct PrimitiveUbo;

struct RenderingUnit{
  std::shared_ptr<api::RenderingPipeline> pipeline;
  std::shared_ptr<geometry::PrimitiveUbo> ubo;
};

class GltfModel {
 public:
  GltfModel() = delete;
  GltfModel(std::shared_ptr<api::RenderingContext> context, const std::string &path);
  void LoadScene();
  void SetCamera(int camera_index);
  void Render();
  void SetViewport(size_t width, size_t height);
 private:
  void LoadNode(const tinygltf::Node &node, glm::mat4 parent_transform = glm::identity<glm::mat4>());
  std::vector<geometry::RenderingUnit> LoadMesh(tinygltf::Mesh &mesh, glm::mat4 model_matrix);
 private:
  std::shared_ptr<api::RenderingContext> context_;
  tinygltf::Model model_{};
  std::vector<std::shared_ptr<api::Buffer>> buffers_{};
  std::vector<std::shared_ptr<api::Texture2D>> textures_{};
  std::vector<RenderingUnit> current_pipelines_{};
  std::vector<Camera> cameras_;
};
}

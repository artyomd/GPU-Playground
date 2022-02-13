//
// Created by Artyom Dangizyan on 12/30/20.
//

#pragma once

#include <tinygltf/tiny_gltf.h>

#include <glm/ext.hpp>
#include <string>

#include "src/api/rendering_context.hpp"

namespace geometry {

struct PrimitiveUbo;
struct ParsedAttribute;

struct RenderingUnit {
  std::shared_ptr<api::RenderingPipeline> pipeline;
  size_t index_count;
  std::shared_ptr<geometry::PrimitiveUbo> ubo;
};

class GltfModel {
 public:
  GltfModel() = delete;
  GltfModel(std::shared_ptr<api::RenderingContext> context,
            const std::string &path);
  std::vector<std::string> GetScenes();
  [[nodiscard]] int GetDefaultSceneIndex() const;
  void LoadScene(uint scene_index);
  void SetCamera(uint camera_index, glm::mat4 view);
  void Render();
  void SetViewport(uint32_t width, uint32_t height);

 private:
  void LoadNode(const tinygltf::Node &node,
                glm::mat4 parent_transform = glm::identity<glm::mat4>());
  std::vector<geometry::RenderingUnit> LoadMesh(tinygltf::Mesh &mesh,
                                                glm::mat4 model_matrix);
  ParsedAttribute ParseAttribute(const std::string &attribute_name,
                                 int accessor_id);

 private:
  std::shared_ptr<api::RenderingContext> context_;
  tinygltf::Model model_;
  std::vector<RenderingUnit> current_pipelines_;
  std::vector<glm::mat4> cameras_;
  std::vector<std::shared_ptr<api::Texture2D>> textures_;
};
}  // namespace geometry

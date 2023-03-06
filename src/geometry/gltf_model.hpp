#pragma once

#include "vulkan/image.hpp"
#include "vulkan/image_view.hpp"
#include "vulkan/rendering_context.hpp"
#include "vulkan/rendering_pipeline.hpp"
#include "vulkan/vertex_buffer_layout.hpp"

#include <glm/ext.hpp>
#include <string>
#include <tiny_gltf.h>

namespace geometry {
typedef std::pair<std::shared_ptr<vulkan::ImageView>, std::shared_ptr<vulkan::Sampler>> CombinedImageSampler;
struct ParsedAttribute {
  unsigned char *data = nullptr;
  size_t stride = 0;
  size_t instance_count = 0;
  unsigned int element_count = 0;
  vulkan::DataType element_type = vulkan::DataType::FLOAT;
  size_t element_size = 0;
};
struct MVP {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
  alignas(16) glm::mat4 normal;
};

struct Material {
  alignas(16) glm::vec4 base_color;
  alignas(16) glm::vec3 emissive_factor;
  alignas(4) float metallic_factor;
  alignas(4) float roughness_factor;
  alignas(4) float alpha_cutoff;
  alignas(4) bool double_sided;
};
struct PrimitiveUbo {
  MVP mvp{};
  Material material{};
};

class RenderingUnit {
 private:
  std::shared_ptr<vulkan::Shader> vertex_shader_;
  std::shared_ptr<vulkan::Shader> fragment_shader_;

  vulkan::VertexBufferLayout vbl_{};
  std::shared_ptr<vulkan::Buffer> vertex_buffer_ = nullptr;

  size_t index_count_ = 0;
  VkIndexType index_type_ = VkIndexType::VK_INDEX_TYPE_NONE_KHR;
  std::shared_ptr<vulkan::Buffer> index_buffer_ = nullptr;

  VkPrimitiveTopology draw_mode_ = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

  std::shared_ptr<vulkan::RenderingPipeline> pipeline_ = nullptr;

  std::map<unsigned int, CombinedImageSampler> textures_;
  std::shared_ptr<vulkan::Buffer> mvp_uniform_buffer_ = nullptr;
  std::shared_ptr<vulkan::Buffer> material_uniform_buffer_ = nullptr;
 public:
  geometry::PrimitiveUbo ubo_data_;
  RenderingUnit(const std::shared_ptr<vulkan::RenderingContext> &context,
                const std::shared_ptr<vulkan::Shader> &vertex_shader,
                const std::shared_ptr<vulkan::Shader> &fragment_shader,
                const vulkan::VertexBufferLayout &vbl,
                const std::shared_ptr<vulkan::Buffer> &vertex_buffer,
                size_t index_count,
                VkIndexType index_type,
                const std::shared_ptr<vulkan::Buffer> &index_buffer,
                VkPrimitiveTopology draw_mode,
                std::map<unsigned int, CombinedImageSampler> textures);

  void RenderPassChanged(std::shared_ptr<vulkan::RenderingContext> context,
                         std::shared_ptr<vulkan::RenderPass> render_pass,
                         size_t descriptor_set_count,
                         VkSampleCountFlagBits sample_count);

  void UpdateMvp();

  void Draw(VkCommandBuffer command_buffer, uint16_t descriptor_set_index) const;
};

class GltfModel {
 public:
  static std::shared_ptr<GltfModel> Create(std::shared_ptr<vulkan::RenderingContext> context, const std::string &path);
  GltfModel() = delete;
  GltfModel(const GltfModel &) = delete;
  std::vector<std::string> GetScenes();
  [[nodiscard]] int GetDefaultSceneIndex() const;
  void LoadScene(uint scene_index);
  void SetCamera(uint camera_index, glm::mat4 view);
  void OnRenderPassChanged(std::shared_ptr<vulkan::RenderPass> render_pass,
                           size_t descriptor_set_count,
                           VkSampleCountFlagBits sample_count);
  void Render(VkCommandBuffer command_buffer, uint16_t descriptor_set_index);
  virtual ~GltfModel();

 private:
  GltfModel(std::shared_ptr<vulkan::RenderingContext> context, const std::string &path);

  void LoadNode(const tinygltf::Node &node,
                glm::mat4 parent_transform = glm::identity<glm::mat4>());
  std::vector<geometry::RenderingUnit> LoadMesh(tinygltf::Mesh &mesh,
                                                glm::mat4 model_matrix);
  ParsedAttribute ParseAttribute(const std::string &attribute_name,
                                 int accessor_id);

 private:
  std::shared_ptr<vulkan::RenderingContext> context_;
  tinygltf::Model model_;
  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  std::vector<RenderingUnit> current_pipelines_;
  std::vector<glm::mat4> cameras_;
  std::vector<std::shared_ptr<vulkan::Image>> images_;
  std::vector<std::shared_ptr<vulkan::Sampler>> samplers_;
  std::vector<CombinedImageSampler> textures_;

  std::shared_ptr<vulkan::RenderPass> render_pass_ = nullptr;
  size_t descriptor_set_count_ = 0;
  VkSampleCountFlagBits sample_count_ = VK_SAMPLE_COUNT_1_BIT;
};
}  // namespace geometry

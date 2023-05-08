#include "lighting.hpp"

#include "vulkan/utils.hpp"

#include <glm/gtx/hash.hpp>
#include <spdlog/spdlog.h>
#include <tiny_obj_loader.h>

#include <cstring>
#include <unordered_map>

namespace {
struct ViewBuffer {
  glm::vec4 eye;
  glm::vec4 light;
};
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  bool operator==(const Vertex &other) const {
    return position == other.position && normal == other.normal;
  }
};
}  // namespace

namespace std {
template<>
struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.position));
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.normal));
    return seed;
  }
};
}  // namespace std

std::shared_ptr<renderable::Lighting> renderable::Lighting::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                                   std::shared_ptr<Menu> parent) {
  return std::shared_ptr<Lighting>(new Lighting(context, parent));
}

renderable::Lighting::Lighting(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent)
    : Model(context, parent, true, true), index_type_(VkIndexType::VK_INDEX_TYPE_UINT32) {

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib,
                        &shapes,
                        &materials,
                        &warn,
                        &err,
                        (RESOURCE_DIR + std::string("/models/dragon.obj")).c_str(),
                        (RESOURCE_DIR + std::string("/models")).c_str())) {
    throw std::runtime_error(warn + err);
  }
  if (!warn.empty()) {
    spdlog::warn(warn);
  }

  if (!err.empty()) {
    spdlog::error(warn);
  }

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::unordered_map<Vertex, uint32_t> unique_vertices;

  for (const auto &kShape : shapes) {
    for (const auto &kIndex : kShape.mesh.indices) {
      Vertex vertex = {};
      vertex.position = {
          attrib.vertices[3 * static_cast<uint64_t>(kIndex.vertex_index) + 0],
          attrib.vertices[3 * static_cast<uint64_t>(kIndex.vertex_index) + 1],
          attrib.vertices[3 * static_cast<uint64_t>(kIndex.vertex_index) + 2]};
      vertex.normal = {
          attrib.normals[3 * static_cast<uint64_t>(kIndex.vertex_index) + 0],
          attrib.normals[3 * static_cast<uint64_t>(kIndex.vertex_index) + 1],
          attrib.normals[3 * static_cast<uint64_t>(kIndex.vertex_index) + 2]};
      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(unique_vertices[vertex]);
    }
  }

  vbl_.Push({0, vulkan::DataType::FLOAT, 3});
  vbl_.Push({1, vulkan::DataType::FLOAT, 3});

  vertex_buffer_ = vulkan::Buffer::Create(context,
                                          vertices.size() * sizeof(Vertex),
                                          VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                                              | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          false);
  index_buffer_ = vulkan::Buffer::Create(context,
                                         indices.size() * sizeof(uint32_t),
                                         VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                                             | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                         false);

  auto command_pool = context->CreateCommandPool();
  {
    auto staging_buffer = vulkan::Buffer::Create(context,
                                                 vertex_buffer_->GetSizeInBytes(),
                                                 VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), vertices.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto command_buffer = context->CreateCommandBuffer(command_pool);
    VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    vertex_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto fence = context->CreateFence();
    context->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context->WaitForFence(fence);
    context->DestroyFence(fence);
    context->FreeCommandBuffer(command_pool, command_buffer);
  }
  {
    auto staging_buffer = vulkan::Buffer::Create(context,
                                                 index_buffer_->GetSizeInBytes(),
                                                 VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), indices.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto command_buffer = context->CreateCommandBuffer(command_pool);
    VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    index_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto fence = context->CreateFence();
    context->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context->WaitForFence(fence);
    context->DestroyFence(fence);
    context->FreeCommandBuffer(command_pool, command_buffer);
  }

  index_count_ = indices.size();

  const std::vector<uint32_t> kVertexShader = {
#include "lighting_vertex.spv"
  };
  v_shader_ = vulkan::Shader::Create(context,
                                     kVertexShader,
                                     "main");

  const std::vector<uint32_t> kFragmentShader = {
#include "lighting_fragment.spv"
  };
  f_shader_ = vulkan::Shader::Create(context,
                                     kFragmentShader,
                                     "main");

  ViewBuffer buffer = {
      .eye = glm::vec4(0.0, 8.0, 0.0, 0.0),
      .light = glm::vec4(3.0, 0.0, 1.0, 0.0),
  };

  lighting_buffer_ = vulkan::Buffer::Create(context,
                                            sizeof(ViewBuffer),
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            true);
  {
    std::memcpy(lighting_buffer_->Map(), &buffer, lighting_buffer_->GetSizeInBytes());
    lighting_buffer_->Unmap();
  }
  context->DestroyCommandPool(command_pool);

}

std::shared_ptr<vulkan::RenderingPipeline> renderable::Lighting::CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                                                std::shared_ptr<vulkan::RenderPass> render_pass,
                                                                                size_t descriptor_set_count,
                                                                                VkSampleCountFlagBits sample_count) {
  auto pipeline = vulkan::RenderingPipeline::Create(rendering_context,
                                                    render_pass,
                                                    v_shader_,
                                                    f_shader_,
                                                    vbl_,
                                                    {
                                                        .draw_mode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                        .cull_mode = VK_CULL_MODE_BACK_BIT,
                                                        .front_face = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                                        .enable_depth_test = VK_TRUE,
                                                        .depth_function = VkCompareOp::VK_COMPARE_OP_LESS,
                                                        .sample_count = sample_count,
                                                    },
                                                    descriptor_set_count);
  pipeline->SetVertexBuffer(vertex_buffer_);
  pipeline->SetIndexBuffer(index_buffer_, index_type_);
  for (size_t descriptor_index = 0; descriptor_index < descriptor_set_count; descriptor_index++) {
    pipeline->SetUniformBuffer(1, descriptor_index, lighting_buffer_);
  }
  return pipeline;
}
size_t renderable::Lighting::NumOfIndicesToDraw() {
  return index_count_;
}
glm::mat4 renderable::Lighting::ComputeViewMatrix() {
  return glm::lookAt(glm::vec3(0.0F, 8.0F, 0.0F), glm::vec3(0.0F, 0.0F, 0.0F),
                     glm::vec3(0.0F, 0.0F, 1.0F));
}
renderable::Lighting::~Lighting() {
  WaitForCommandBuffersToFinish();
}

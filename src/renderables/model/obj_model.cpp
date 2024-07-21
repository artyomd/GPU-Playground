#include "obj_model.hpp"

#include "vulkan/utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <cstring>
#include <unordered_map>

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
template<>
struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    glm::detail::hash_combine(seed, hash<glm::vec3>()(vertex.position));
    glm::detail::hash_combine(seed, hash<glm::vec2>()(vertex.tex_coord));
    return seed;
  }
};
}  // namespace std

std::shared_ptr<renderable::ObjModel> renderable::ObjModel::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                                   std::shared_ptr<Menu> parent) {
  return std::shared_ptr<ObjModel>(new ObjModel(context, parent));
}

renderable::ObjModel::ObjModel(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent)
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
                        (RESOURCE_DIR + std::string("/models/chalet.obj")).c_str(),
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
          attrib.vertices[3 * static_cast<uint64_t>(kIndex.vertex_index) + 2]
      };
      vertex.tex_coord = {
          attrib.texcoords[2 * static_cast<uint64_t>(kIndex.texcoord_index) + 0],
          attrib.texcoords[2 * static_cast<uint64_t>(kIndex.texcoord_index) + 1]
      };
      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(unique_vertices[vertex]);
    }
  }

  vbl_.Push({0, vulkan::DataType::FLOAT, 3});
  vbl_.Push({1, vulkan::DataType::FLOAT, 2});

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
#include "texture2d_vertex.spv"
  };
  v_shader_ = vulkan::Shader::Create(context,
                                     kVertexShader,
                                     "main");

  const std::vector<uint32_t> kFragmentShader = {
#include "texture2d_fragment.spv"
  };
  f_shader_ = vulkan::Shader::Create(context,
                                     kFragmentShader,
                                     "main");

  stbi_set_flip_vertically_on_load(true);
  int tex_width = 0, tex_height = 0, tex_channels = 0;
  stbi_uc *pixels = stbi_load((RESOURCE_DIR + std::string("/textures/chalet.jpg")).c_str(),
                              &tex_width,
                              &tex_height,
                              &tex_channels,
                              STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }

  auto image = vulkan::Image::Create(context,
                                     tex_width,
                                     tex_height,
                                     VK_FORMAT_R8G8B8A8_UNORM,
                                     VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                     false,
                                     VK_SAMPLE_COUNT_1_BIT);
  {
    auto staging_buffer = vulkan::Buffer::Create(context,
                                                 image->GetSizeInBytes(),
                                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), pixels, staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    stbi_image_free(pixels);
    auto command_buffer = context->CreateCommandBuffer(command_pool);
    VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    image->CopyFrom(command_buffer, staging_buffer);
    image->TransferTo(command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto fence = context->CreateFence();
    context->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context->WaitForFence(fence);
    context->DestroyFence(fence);
    context->FreeCommandBuffer(command_pool, command_buffer);
  }
  context->DestroyCommandPool(command_pool);
  sampler_ = vulkan::Sampler::Create(context,
                                     VK_FILTER_LINEAR,
                                     VK_FILTER_LINEAR,
                                     VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                                     VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
  image_view_ = vulkan::ImageView::Create(context, image);

}

std::shared_ptr<vulkan::RenderingPipeline> renderable::ObjModel::CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
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
    pipeline->SetImageView(1, descriptor_index, image_view_, sampler_);
  }
  return pipeline;
}
size_t renderable::ObjModel::NumOfIndicesToDraw() {
  return index_count_;
}
glm::mat4 renderable::ObjModel::ComputeViewMatrix() {
  return glm::lookAt(glm::vec3(-2.0F, 2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 0.0F),
                     glm::vec3(0.0F, 0.0F, 1.0F));
}
renderable::ObjModel::~ObjModel() {
  WaitForCommandBuffersToFinish();
}

#include "cube.hpp"

#include <cstring>
#include <geometry/point.hpp>

#include "vulkan/utils.hpp"

std::shared_ptr<renderable::Cube> renderable::Cube::Create(const std::shared_ptr<vulkan::RenderingContext> &context,
                                                           const std::shared_ptr<Menu> &parent) {
  return std::shared_ptr<Cube>(new Cube(context, parent));
}

renderable::Cube::Cube(const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent)
    : Model(context, parent, true, true), index_type_(VK_INDEX_TYPE_UINT16) {
  std::vector positions = {
      geometry::PointWithColor(-0.5, -0.5, 0.5, 255, 0, 0, 255),
      geometry::PointWithColor(0.5, -0.5, 0.5, 0, 255, 0, 255),
      geometry::PointWithColor(0.5, 0.5, 0.5, 0, 0, 255, 255),
      geometry::PointWithColor(-0.5, 0.5, 0.5, 255, 255, 255, 255),
      geometry::PointWithColor(-0.5, -0.5, -0.5, 255, 0, 0, 255),
      geometry::PointWithColor(0.5, -0.5, -0.5, 0, 255, 0, 255),
      geometry::PointWithColor(0.5, 0.5, -0.5, 0, 0, 255, 255),
      geometry::PointWithColor(-0.5, 0.5, -0.5, 255, 255, 255, 255),
  };
  std::vector<uint16_t> indices{0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7,
                                4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};

  vbl_.Push({.binding_index = 0, .type = vulkan::DataType::FLOAT, .count = 3});
  vbl_.Push({.binding_index = 1, .type = vulkan::DataType::UINT_8, .count = 4});

  vertex_buffer_ = vulkan::Buffer::Create(context, positions.size() * sizeof(decltype(positions)::value_type),
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
  index_buffer_ = vulkan::Buffer::Create(context, indices.size() * sizeof(decltype(indices)::value_type),
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

  auto *command_pool = context->CreateCommandPool();
  {
    const auto staging_buffer =
        vulkan::Buffer::Create(context, vertex_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), positions.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto *command_buffer = context->CreateCommandBuffer(command_pool);
    constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    vertex_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto *fence = context->CreateFence();
    context->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context->WaitForFence(fence);
    context->DestroyFence(fence);
    context->FreeCommandBuffer(command_pool, command_buffer);
  }
  {
    const auto staging_buffer =
        vulkan::Buffer::Create(context, index_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), indices.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto *command_buffer = context->CreateCommandBuffer(command_pool);
    constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    index_buffer_->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto *fence = context->CreateFence();
    context->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context->WaitForFence(fence);
    context->DestroyFence(fence);
    context->FreeCommandBuffer(command_pool, command_buffer);
  }
  context->DestroyCommandPool(command_pool);

  index_count_ = indices.size();

  const std::vector<uint32_t> kVertexShader = {
#include "default_mvp_color_vertex_shader.spv"

  };
  v_shader_ = vulkan::Shader::Create(context, kVertexShader, "main");

  const std::vector<uint32_t> kFragmentShader = {
#include "default_color_fragment_shader.spv"

  };
  f_shader_ = vulkan::Shader::Create(context, kFragmentShader, "main");
}

std::shared_ptr<vulkan::RenderingPipeline> renderable::Cube::CreatePipeline(
    const std::shared_ptr<vulkan::RenderingContext> &rendering_context,
    const std::shared_ptr<vulkan::RenderPass> &render_pass, const size_t &descriptor_set_count,
    const VkSampleCountFlagBits &sample_count) {
  auto pipeline = vulkan::RenderingPipeline::Create(rendering_context, render_pass, v_shader_, f_shader_, vbl_,
                                                    {
                                                        .draw_mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                        .cull_mode = VK_CULL_MODE_BACK_BIT,
                                                        .front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                                        .enable_depth_test = VK_TRUE,
                                                        .depth_function = VK_COMPARE_OP_LESS,
                                                        .sample_count = sample_count,
                                                    },
                                                    descriptor_set_count);
  pipeline->SetVertexBuffer(vertex_buffer_);
  pipeline->SetIndexBuffer(index_buffer_, index_type_);
  return pipeline;
}
size_t renderable::Cube::NumOfIndicesToDraw() { return index_count_; }
renderable::Cube::~Cube() { WaitForCommandBuffersToFinish(); }

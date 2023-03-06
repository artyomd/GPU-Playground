#include "texture_2d.hpp"

#include "vulkan/utils.hpp"

#include <stb_image.h>

#include <cstring>

std::shared_ptr<renderable::Texture2d> renderable::Texture2d::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                                     std::shared_ptr<Menu> parent) {
  auto triangle = new Texture2d(context, parent);
  return std::shared_ptr<Texture2d>(triangle);
}

renderable::Texture2d::Texture2d(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent)
    : Model(context, parent), index_buffer_data_type_(VkIndexType::VK_INDEX_TYPE_UINT16), index_count_(6) {
  stbi_set_flip_vertically_on_load(false);
  int tex_width = 0, tex_height = 0, tex_channels = 0;
  stbi_uc *pixels = stbi_load((RESOURCE_DIR + std::string("/textures/texture2d.jpg")).c_str(), &tex_width,
                              &tex_height, &tex_channels, STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }
  float scaled_width = 0;
  float scaled_height = 0;
  if (tex_width > tex_height) {
    scaled_width = 2.0f;
    scaled_height = static_cast<float>(tex_height * scaled_width) / static_cast<float>(tex_width);
  } else {
    scaled_height = 2.0f;
    scaled_width = static_cast<float>(tex_width * scaled_height) / static_cast<float>(tex_height);
  }

  std::array<float, 16> vertex_data = {
      -scaled_width, scaled_height, 0.0F, 0.0F,
      scaled_width, scaled_height, 1.0F, 0.0F,
      -scaled_width, -scaled_height, 0.0F, 1.0F,
      scaled_width, -scaled_height, 1.0F, 1.0F
  };

  std::array<uint16_t, 6> index_data = {0, 1, 2, 2, 1, 3};

  vertex_buffer_layout_.Push({0, vulkan::DataType::FLOAT, 2});
  vertex_buffer_layout_.Push({1, vulkan::DataType::FLOAT, 2});

  vertex_buffer_ = vulkan::Buffer::Create(context,
                                          vertex_data.size() * sizeof(float),
                                          VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
                                              | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                          false);
  index_buffer_ = vulkan::Buffer::Create(context,
                                         index_data.size() * sizeof(uint16_t),
                                         VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT
                                             | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                         false);

  auto command_pool = context->CreateCommandPool();
  {
    auto staging_buffer = vulkan::Buffer::Create(context,
                                                 vertex_buffer_->GetSizeInBytes(),
                                                 VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 true);
    std::memcpy(staging_buffer->Map(), vertex_data.data(), staging_buffer->GetSizeInBytes());
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
    std::memcpy(staging_buffer->Map(), index_data.data(), staging_buffer->GetSizeInBytes());
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

  v_shader_ = vulkan::Shader::Create(context,
                                     SHADER_DIR + std::string("texture2d_vertex.glsl"),
                                     "main",
                                     VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
  f_shader_ = vulkan::Shader::Create(context,
                                     SHADER_DIR + std::string("texture2d_fragment.glsl"),
                                     "main",
                                     VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

  auto texture_2d = vulkan::Image::Create(context,
                                          tex_width,
                                          tex_height,
                                          VK_FORMAT_R8G8B8A8_UNORM,
                                          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                          false,
                                          VK_SAMPLE_COUNT_1_BIT);
  {
    auto staging_buffer = vulkan::Buffer::Create(context,
                                                 texture_2d->GetSizeInBytes(),
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
    texture_2d->CopyFrom(command_buffer, staging_buffer);
    texture_2d->TransferTo(command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
  image_view_ = vulkan::ImageView::Create(context, texture_2d);
}

std::shared_ptr<vulkan::RenderingPipeline> renderable::Texture2d::CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                                                 std::shared_ptr<vulkan::RenderPass> render_pass,
                                                                                 size_t descriptor_set_count,
                                                                                 VkSampleCountFlagBits sample_count) {
  auto pipeline = vulkan::RenderingPipeline::Create(rendering_context,
                                                    render_pass,
                                                    v_shader_,
                                                    f_shader_,
                                                    vertex_buffer_layout_,
                                                    {
                                                        .draw_mode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                        .cull_mode = VK_CULL_MODE_NONE,
                                                        .front_face = VkFrontFace::VK_FRONT_FACE_CLOCKWISE,
                                                        .enable_depth_test = VK_FALSE,
                                                        .depth_function = VkCompareOp::VK_COMPARE_OP_LESS,
                                                        .sample_count = sample_count,
                                                    },
                                                    descriptor_set_count);
  pipeline->SetVertexBuffer(vertex_buffer_);
  pipeline->SetIndexBuffer(index_buffer_, index_buffer_data_type_);
  for (size_t descriptor_index = 0; descriptor_index < descriptor_set_count; descriptor_index++) {
    pipeline->SetImageView(1, descriptor_index, image_view_, sampler_);
  }
  return pipeline;
}
size_t renderable::Texture2d::NumOfIndicesToDraw() {
  return index_count_;
}
renderable::Texture2d::~Texture2d() {
  WaitForCommandBuffersToFinish();
}

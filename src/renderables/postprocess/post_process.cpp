#include "post_process.hpp"

#include <spdlog/spdlog.h>
#include <stb_image.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <ranges>

#include "vulkan/utils.hpp"

std::shared_ptr<renderable::PostProcess> renderable::PostProcess::Create(
    const std::shared_ptr<vulkan::RenderingContext>& context, const std::shared_ptr<Menu>& parent) {
  return std::shared_ptr<PostProcess>(new PostProcess(context, parent));
}
renderable::PostProcess::PostProcess(const std::shared_ptr<vulkan::RenderingContext>& context,
                                     const std::shared_ptr<Menu>& parent)
    : rendering_context_(context),
      parent_(parent),
      command_pool_(rendering_context_->CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {
  vbl_.Push({.binding_index = 0, .type = vulkan::DataType::FLOAT, .count = 2});
  vbl_.Push({.binding_index = 1, .type = vulkan::DataType::FLOAT, .count = 2});

  const std::array<uint16_t, 6> index_data = {0, 1, 2, 1, 2, 3};
  index_buffer_ = vulkan::Buffer::Create(context, index_data.size() * sizeof(decltype(index_data)::value_type),
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

  std::array vertex_data = {-0.5f, 0.5f,  0.0f, 0.0f, 0.5f, 0.5f,  1.0f, 0.0f,
                            -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 1.0f, 1.0f};
  vertex_buffer_ =
      vulkan::Buffer::Create(this->rendering_context_, vertex_data.size() * sizeof(decltype(vertex_data)::value_type),
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

  {
    const auto index_staging_buffer = vulkan::Buffer::Create(rendering_context_, index_buffer_->GetSizeInBytes(),
                                                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(index_staging_buffer->Map(), index_data.data(), index_staging_buffer->GetSizeInBytes());
    index_staging_buffer->Unmap();

    const auto vertex_staging_buffer = vulkan::Buffer::Create(rendering_context_, vertex_buffer_->GetSizeInBytes(),
                                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(vertex_staging_buffer->Map(), vertex_data.data(), vertex_staging_buffer->GetSizeInBytes());
    vertex_staging_buffer->Unmap();

    auto* command_pool = rendering_context_->CreateCommandPool();
    auto* command_buffer = rendering_context_->CreateCommandBuffer(command_pool);
    constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    index_buffer_->CopyFrom(command_buffer, index_staging_buffer, index_staging_buffer->GetSizeInBytes());
    vertex_buffer_->CopyFrom(command_buffer, vertex_staging_buffer, vertex_staging_buffer->GetSizeInBytes());
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto* fence = rendering_context_->CreateFence();
    rendering_context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE,
                                            fence);
    rendering_context_->WaitForFence(fence);
    rendering_context_->DestroyFence(fence);
    rendering_context_->FreeCommandBuffer(command_pool, command_buffer);
    rendering_context_->DestroyCommandPool(command_pool);
  }
  const std::vector<uint32_t> vertex_shader = {
#include "texture2d_vertex.spv"

  };
  v_shader_ = vulkan::Shader::Create(context, vertex_shader, "main");

  const std::vector<uint32_t> fragment_shader = {
#include "texture2d_fragment.spv"

  };
  f_shader_ = vulkan::Shader::Create(context, fragment_shader, "main");
  sampler_ = vulkan::Sampler::Create(rendering_context_, VK_FILTER_LINEAR, VK_FILTER_LINEAR,
                                     VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
  UpdateImage();
}

std::shared_ptr<vulkan::Image> renderable::PostProcess::LoadImage(const std::string& path) const {
  stbi_set_flip_vertically_on_load(0);
  int tex_width = 0;
  int tex_height = 0;
  int tex_channels = 0;
  stbi_uc* pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }
  const auto image =
      vulkan::Image::Create(this->rendering_context_, tex_width, tex_height, VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, false, VK_SAMPLE_COUNT_1_BIT);
  {
    const auto staging_buffer =
        vulkan::Buffer::Create(rendering_context_, image->GetSizeInBytes(), VK_IMAGE_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), pixels, staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    stbi_image_free(pixels);
    auto* command_buffer = rendering_context_->CreateCommandBuffer(command_pool_);
    constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    image->CopyFrom(command_buffer, staging_buffer);
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto* fence = rendering_context_->CreateFence();
    rendering_context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE,
                                            fence);
    rendering_context_->WaitForFence(fence);
    rendering_context_->DestroyFence(fence);
    rendering_context_->FreeCommandBuffer(command_pool_, command_buffer);
  }
  return image;
}
void renderable::PostProcess::UpdateImage() {
  rendering_context_->WaitForQueueIdle();
  const auto image = LoadImage(this->image_file_);
  source_image_ = vulkan::ImageView::Create(rendering_context_, image);
  for (auto& buffer : command_buffers_ | std::views::values) {
    buffer.params_hash = 0;
  }
  const auto width = static_cast<float>(image->GetWidth());
  const auto height = static_cast<float>(image->GetHeight());
  if (this->image_size_ == glm::vec2{width, height}) {
    return;
  }
  this->image_size_ = {width, height};
  for (auto& buffer : command_buffers_ | std::views::values) {
    const auto dst =
        vulkan::Image::Create(rendering_context_, image->GetWidth(), image->GetHeight(), image->GetPixelFormat(),
                              VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, false, VK_SAMPLE_COUNT_1_BIT);
    buffer.processed_image = vulkan::ImageView::Create(rendering_context_, dst);
  }
  UpdateModelMatrix();
}

void renderable::PostProcess::UpdateModelMatrix() {
  const auto scale = glm::min(window_size_[0] / image_size_[0], window_size_[1] / image_size_[1]);
  ubo_.model = glm::scale(glm::mat4(1.0f), glm::vec3(image_size_[0] * scale, image_size_[1] * scale, 1.0f));
}

void renderable::PostProcess::CleanupCommandBuffers() {
  for (const auto& command_buffer : command_buffers_ | std::views::values) {
    rendering_context_->WaitForFence(command_buffer.fence);
    rendering_context_->DestroyFence(command_buffer.fence);
    rendering_context_->FreeCommandBuffer(command_pool_, command_buffer.command_buffer);
  }
  command_buffers_.clear();
}

void renderable::PostProcess::SetupImages(const std::vector<std::shared_ptr<vulkan::Image>>& images) {
  framebuffers_.clear();
  auto image_format = images[0]->GetPixelFormat();
  if (imgui_wrapper_ != nullptr && imgui_wrapper_->GetImageCount() != images.size()) {
    imgui_wrapper_ = nullptr;
  }
  if (blur_ == nullptr || blur_->GetDescriptorCount() != images.size()) {
    blur_ = Blur::Create(rendering_context_, images.size());
  }
  if (render_pass_ == nullptr || render_pass_->GetColorAttachmentFormat() != image_format) {
    render_pass_ = std::make_shared<vulkan::RenderPass>(rendering_context_, image_format);
    imgui_wrapper_ = nullptr;
    pipeline_ = nullptr;
  }
  // imgui
  if (imgui_wrapper_ == nullptr) {
    imgui_wrapper_ = std::make_shared<ImguiWrapper>(rendering_context_, render_pass_, images.size());
  }
  if (pipeline_ == nullptr) {
    pipeline_ = vulkan::RenderingPipeline::Create(rendering_context_, render_pass_, v_shader_, f_shader_, vbl_,
                                                  {
                                                      .draw_mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                      .cull_mode = VK_CULL_MODE_NONE,
                                                      .front_face = VK_FRONT_FACE_CLOCKWISE,
                                                      .enable_depth_test = VK_FALSE,
                                                      .depth_function = VK_COMPARE_OP_LESS,
                                                      .sample_count = VK_SAMPLE_COUNT_1_BIT,
                                                  },
                                                  images.size());
    pipeline_->SetVertexBuffer(0, vertex_buffer_);
    pipeline_->SetIndexBuffer(index_buffer_, VK_INDEX_TYPE_UINT16);
  }
  CleanupCommandBuffers();
  uint32_t descriptor_index = 0;
  for (const auto& image : images) {
    auto image_view = vulkan::ImageView::Create(rendering_context_, image);
    const auto framebuffer = vulkan::Framebuffer::Create(rendering_context_, render_pass_, {image_view});
    framebuffers_[image] = framebuffer;

    const auto uniform_buffer = vulkan::Buffer::Create(rendering_context_, sizeof(UniformBufferObject),
                                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true);
    const auto source = source_image_->GetImage();
    const auto dst =
        vulkan::Image::Create(rendering_context_, source->GetWidth(), source->GetHeight(), source->GetPixelFormat(),
                              VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, false, VK_SAMPLE_COUNT_1_BIT);
    command_buffers_[image] = {.fence = rendering_context_->CreateFence(true),
                               .command_buffer = rendering_context_->CreateCommandBuffer(command_pool_),
                               .uniform_buffer = uniform_buffer,
                               .descriptor_index = descriptor_index,
                               .params_hash = 0,
                               .processed_image = vulkan::ImageView::Create(rendering_context_, dst)};
    pipeline_->SetUniformBuffer(0, descriptor_index, uniform_buffer);
    descriptor_index++;
  }
  const auto width = static_cast<float>(images[0]->GetWidth());
  const auto height = static_cast<float>(images[0]->GetHeight());
  this->window_size_ = {width, height};
  UpdateModelMatrix();
  ubo_.proj = glm::ortho<float>(-(width / 2.0f), width / 2.0f, (height / 2.0f), -(height / 2.0f));
}
void renderable::PostProcess::Render(const std::shared_ptr<vulkan::Image>& image, const VkSemaphore& wait_semaphore,
                                     const VkSemaphore& signal_semaphore) {
  auto execution_context = command_buffers_[image];
  const auto framebuffer = framebuffers_[image];
  rendering_context_->WaitForFence(execution_context.fence);
  rendering_context_->ResetFence(execution_context.fence);

  {
    constexpr VkCommandBufferBeginInfo begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(execution_context.command_buffer, &begin_info);

    const auto processed_image = execution_context.processed_image;
    if (execution_context.params_hash != blur_->ParamsHash()) {
      execution_context.params_hash = blur_->Process(execution_context.command_buffer, source_image_, processed_image,
                                                     execution_context.descriptor_index);
    }

    {
      memcpy(execution_context.uniform_buffer->Map(), &ubo_, sizeof(ubo_));
      execution_context.uniform_buffer->Unmap();
    }

    pipeline_->SetImageView(1, execution_context.descriptor_index, processed_image, sampler_);
    processed_image->GetImage()->TransferTo(execution_context.command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VkClearValue clear_value = {
        .color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}},
    };
    const VkRenderPassBeginInfo render_pass_begin_info{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = this->render_pass_->GetRenderPass(),
        .framebuffer = framebuffer->GetFramebuffer(),
        .renderArea =
            {
                .offset = {0, 0},
                .extent =
                    {
                        .width = framebuffer->GetWidth(),
                        .height = framebuffer->GetHeight(),
                    },
            },
        .clearValueCount = 1,
        .pClearValues = &clear_value,
    };
    vkCmdBeginRenderPass(execution_context.command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    const VkViewport viewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(framebuffer->GetWidth()),
        .height = static_cast<float>(framebuffer->GetHeight()),
        .minDepth = 0,
        .maxDepth = 1.0,
    };
    const VkRect2D scissor_rect{.offset =
                                    {
                                        .x = 0,
                                        .y = 0,
                                    },
                                .extent = {
                                    .width = framebuffer->GetWidth(),
                                    .height = framebuffer->GetHeight(),
                                }};
    vkCmdSetViewport(execution_context.command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(execution_context.command_buffer, 0, 1, &scissor_rect);
    pipeline_->DrawIndexed(execution_context.command_buffer, 6, execution_context.descriptor_index);

    ImguiWrapper::BeginFrame();
    ImguiWrapper::ShowFPSOverlay();
    ImGui::Begin("Post process effects");
    if (ImGui::Button("open file dialog")) {
      file_dialog_.SetDirectory(std::filesystem::path(image_file_).parent_path());
      file_dialog_.Open();
    }
    blur_->RenderParams();
    if (const auto locked_parent = parent_.lock(); locked_parent != nullptr) {
      if (ImGui::Button("<----")) {
        locked_parent->Pop();
      }
    }
    ImGui::End();
    file_dialog_.Display();

    ImguiWrapper::EndFrameAndDraw(execution_context.command_buffer);
    vkCmdEndRenderPass(execution_context.command_buffer);
    vkEndCommandBuffer(execution_context.command_buffer);
    rendering_context_->SubmitCommandBuffer(execution_context.command_buffer, wait_semaphore,
                                            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, signal_semaphore,
                                            execution_context.fence);
    if (file_dialog_.HasSelected()) {
      image_file_ = file_dialog_.GetSelected().string();
      file_dialog_.ClearSelected();
      UpdateImage();
    }
  }
}

renderable::PostProcess::~PostProcess() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}

#include "particles.hpp"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <random>
#include <ranges>

#include "vulkan/compute_pipeline.hpp"
#include "vulkan/utils.hpp"

std::shared_ptr<renderable::Particles> renderable::Particles::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent) {
  return std::shared_ptr<Particles>(new Particles(context, parent));
}

renderable::Particles::Particles(const std::shared_ptr<vulkan::RenderingContext> &context,
                                 const std::shared_ptr<Menu> &parent)
    : rendering_context_(context),
      parent_(parent),
      command_pool_(rendering_context_->CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {
  vbl_[0] = vulkan::VertexBufferLayout();
  vbl_[0].Push({.binding_index = 0, .type = vulkan::DataType::FLOAT, .count = 2});
  vbl_[1] = vulkan::VertexBufferLayout();
  vbl_[1].Push({.binding_index = 1, .type = vulkan::DataType::FLOAT, .count = 4});

  const std::vector<uint32_t> compute_shader = {
#include "particles.comp.spv"

  };
  c_shader_ = vulkan::Shader::Create(context, compute_shader, "main");

  const std::vector<uint32_t> vertex_shader = {
#include "point.vert.spv"

  };
  v_shader_ = vulkan::Shader::Create(context, vertex_shader, "main");

  const std::vector<uint32_t> fragment_shader = {
#include "point.frag.spv"

  };
  f_shader_ = vulkan::Shader::Create(context, fragment_shader, "main");

  std::vector<glm::vec2> initial_positions(particle_count_);
  std::vector<glm::vec4> color_data(particle_count_);
  std::vector<glm::vec2> velocity_data(particle_count_);
  {
    std::default_random_engine random_engine(std::random_device{}());
    std::uniform_real_distribution random_dist(0.0f, 1.0f);
    for (uint32_t i = 0; i < particle_count_; i++) {
      constexpr uint32_t height = 600;
      constexpr uint32_t width = 800;
      const float r = 0.25f * sqrt(random_dist(random_engine));
      const float theta = random_dist(random_engine) * 2.0f * 3.14159265358979323846f;
      const float x = r * cos(theta) * height / width;
      const float y = r * sin(theta);
      initial_positions[i] = glm::vec2(x, y);
      velocity_data[i] = normalize(glm::vec2(x, y)) * 0.00025f;
      color_data[i] =
          glm::vec4(random_dist(random_engine), random_dist(random_engine), random_dist(random_engine), 1.0f);
    }

    initial_positions_buffer_ = vulkan::Buffer::Create(
        rendering_context_, initial_positions.size() * sizeof(decltype(initial_positions)::value_type),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        false);
    initial_velocity_buffer_ =
        vulkan::Buffer::Create(rendering_context_, velocity_data.size() * sizeof(decltype(velocity_data)::value_type),
                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);
    color_buffer_ =
        vulkan::Buffer::Create(rendering_context_, color_data.size() * sizeof(decltype(color_data)::value_type),
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

    {
      auto *command_pool = rendering_context_->CreateCommandPool();
      const auto position_staging = vulkan::Buffer::Create(
          rendering_context_, initial_positions_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
      std::memcpy(position_staging->Map(), initial_positions.data(), position_staging->GetSizeInBytes());
      position_staging->Unmap();
      const auto color_staging = vulkan::Buffer::Create(rendering_context_, color_buffer_->GetSizeInBytes(),
                                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
      std::memcpy(color_staging->Map(), color_data.data(), color_staging->GetSizeInBytes());
      color_staging->Unmap();
      const auto velocity_staging = vulkan::Buffer::Create(
          rendering_context_, initial_velocity_buffer_->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
      std::memcpy(velocity_staging->Map(), velocity_data.data(), velocity_staging->GetSizeInBytes());
      velocity_staging->Unmap();

      auto *command_buffer = rendering_context_->CreateCommandBuffer(command_pool);
      constexpr VkCommandBufferBeginInfo command_buffer_begin_info{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = nullptr,
      };
      VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
      color_buffer_->CopyFrom(command_buffer, color_staging, color_buffer_->GetSizeInBytes());
      initial_velocity_buffer_->CopyFrom(command_buffer, velocity_staging, initial_velocity_buffer_->GetSizeInBytes());
      initial_positions_buffer_->CopyFrom(command_buffer, position_staging,
                                          initial_positions_buffer_->GetSizeInBytes());
      VK_CALL(vkEndCommandBuffer(command_buffer));
      auto *fence = rendering_context_->CreateFence();
      rendering_context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE,
                                              fence);
      rendering_context_->WaitForFence(fence);
      rendering_context_->DestroyFence(fence);
      rendering_context_->FreeCommandBuffer(command_pool, command_buffer);
      rendering_context_->DestroyCommandPool(command_pool);
    }
    last_vertex_buffer_ = initial_positions_buffer_;
    last_velocity_buffer_ = initial_velocity_buffer_;
  }
}
void renderable::Particles::CleanupCommandBuffers() {
  for (const auto &command_buffer : command_buffers_ | std::views::values) {
    rendering_context_->WaitForFence(command_buffer.fence);
    rendering_context_->DestroyFence(command_buffer.fence);
    rendering_context_->FreeCommandBuffer(command_pool_, command_buffer.command_buffer);
  }
  command_buffers_.clear();
}

void renderable::Particles::SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) {
  framebuffers_.clear();
  auto image_format = images[0]->GetPixelFormat();
  if (imgui_wrapper_ != nullptr && imgui_wrapper_->GetImageCount() != images.size()) {
    imgui_wrapper_ = nullptr;
  }
  if (render_pipline_ != nullptr && render_pipline_->GetDescriptorSetCount() != images.size()) {
    render_pipline_ = nullptr;
  }
  if (compute_pipeline_ != nullptr && compute_pipeline_->GetDescriptorSetCount() != images.size()) {
    compute_pipeline_ = nullptr;
  }
  if (render_pass_ == nullptr || render_pass_->GetColorAttachmentFormat() != image_format) {
    render_pass_ = std::make_shared<vulkan::RenderPass>(rendering_context_, image_format);
    imgui_wrapper_ = nullptr;
  }
  // imgui
  if (imgui_wrapper_ == nullptr) {
    imgui_wrapper_ = std::make_shared<ImguiWrapper>(rendering_context_, render_pass_, images.size());
  }
  // compute pipline
  if (compute_pipeline_ == nullptr) {
    compute_pipeline_ = vulkan::ComputePipeline::Create(rendering_context_, c_shader_, images.size());
  }
  if (render_pipline_ == nullptr) {
    constexpr vulkan::RenderingPipelineConfig config{
        .draw_mode = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        .cull_mode = VK_CULL_MODE_BACK_BIT,
        .front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .enable_depth_test = VK_FALSE,
        .depth_function = VK_COMPARE_OP_LESS,
        .sample_count = VK_SAMPLE_COUNT_1_BIT,
        .color_blend_attachment_state = {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                              VK_COLOR_COMPONENT_A_BIT,
        }};
    render_pipline_ = vulkan::RenderingPipeline::Create(rendering_context_, render_pass_, v_shader_, f_shader_, vbl_,
                                                        config, images.size());
    render_pipline_->SetVertexBuffer(1, color_buffer_);
  }

  for (const auto &image : images) {
    auto image_view = vulkan::ImageView::Create(rendering_context_, image);
    const auto framebuffer = vulkan::Framebuffer::Create(rendering_context_, render_pass_, {image_view});
    framebuffers_[image] = framebuffer;
  }
  CleanupCommandBuffers();
  uint32_t descriptor_index = 0;
  for (const auto &image : images) {
    command_buffers_[image] = {
        .fence = rendering_context_->CreateFence(true),
        .command_buffer = rendering_context_->CreateCommandBuffer(command_pool_),
        .descriptor_index = descriptor_index,
        .uniform_buffer = vulkan::Buffer::Create(rendering_context_, sizeof(UniformBufferObject),
                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true),
        .vertex_ssbo =
            vulkan::Buffer::Create(rendering_context_, initial_positions_buffer_->GetSizeInBytes(),
                                   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false),
        .velocity_ssbo = vulkan::Buffer::Create(rendering_context_, initial_velocity_buffer_->GetSizeInBytes(),
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, false),
    };
    descriptor_index++;
  }
}

void renderable::Particles::Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &wait_semaphore,
                                   const VkSemaphore &signal_semaphore) {
  const auto execution_context = command_buffers_[image];
  const auto framebuffer = framebuffers_[image];
  rendering_context_->WaitForFence(execution_context.fence);
  rendering_context_->ResetFence(execution_context.fence);
  // update ubo
  {
    const auto current_time = std::chrono::system_clock::now();
    const auto delta = current_time - last_draw_time_;
    last_draw_time_ = current_time;
    ubo_.delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
    memcpy(execution_context.uniform_buffer->Map(), &ubo_, sizeof(ubo_));
    execution_context.uniform_buffer->Unmap();
  }
  {
    constexpr VkCommandBufferBeginInfo begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(execution_context.command_buffer, &begin_info);

    compute_pipeline_->SetBuffer(0, execution_context.descriptor_index, execution_context.uniform_buffer);
    compute_pipeline_->SetBuffer(1, execution_context.descriptor_index, last_vertex_buffer_);
    compute_pipeline_->SetBuffer(2, execution_context.descriptor_index, last_velocity_buffer_);
    compute_pipeline_->SetBuffer(3, execution_context.descriptor_index, execution_context.vertex_ssbo);
    compute_pipeline_->SetBuffer(4, execution_context.descriptor_index, execution_context.velocity_ssbo);
    compute_pipeline_->Dispatch(execution_context.command_buffer, particle_count_ / 256, 1, 1,
                                execution_context.descriptor_index);
    last_vertex_buffer_ = execution_context.vertex_ssbo;
    last_velocity_buffer_ = execution_context.velocity_ssbo;

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

    render_pipline_->SetVertexBuffer(0, last_vertex_buffer_);
    render_pipline_->Draw(execution_context.command_buffer, particle_count_, execution_context.descriptor_index);
    ImguiWrapper::BeginFrame();
    ImguiWrapper::ShowFPSOverlay();
    ImGui::Begin("Particles");
    if (const auto locked_parent = parent_.lock(); locked_parent != nullptr) {
      if (ImGui::Button("<----")) {
        locked_parent->Pop();
      }
    }
    ImGui::End();
    ImguiWrapper::EndFrameAndDraw(execution_context.command_buffer);
    vkCmdEndRenderPass(execution_context.command_buffer);
    vkEndCommandBuffer(execution_context.command_buffer);
    rendering_context_->SubmitCommandBuffer(execution_context.command_buffer, wait_semaphore,
                                            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, signal_semaphore,
                                            execution_context.fence);
    rendering_context_->WaitForFence(execution_context.fence);
  }
}

renderable::Particles::~Particles() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}

#include "shader.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "vulkan/rendering_pipeline.hpp"
#include "vulkan/utils.hpp"

std::shared_ptr<renderable::Shader> renderable::Shader::Create(const std::shared_ptr<vulkan::RenderingContext> &context,
                                                               const std::shared_ptr<Menu> &parent,
                                                               const std::vector<uint32_t> &fragment_shader) {
  return std::shared_ptr<Shader>(new Shader(context, parent, fragment_shader));
}

renderable::Shader::Shader(const std::shared_ptr<vulkan::RenderingContext> &context,
                           const std::shared_ptr<Menu> &parent, const std::vector<uint32_t> &fragment_shader)
    : rendering_context_(context),
      parent_(parent),
      command_pool_(rendering_context_->CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {
  geometry::Point point_0 = {.x = -1.0f, .y = -1.0f, .z = 0.0f};
  geometry::Point point_1 = {.x = 1.0f, .y = -1.0f, .z = 0.0f};
  geometry::Point point_2 = {.x = 1.0f, .y = 1.0f, .z = 0.0f};
  geometry::Point point_3 = {.x = -1.0f, .y = 1.0f, .z = 0.0f};

  quad_ = std::make_shared<geometry::Quad>(rendering_context_, point_0, point_1, point_2, point_3);

  const std::vector<uint32_t> kVertexShader = {
#include "default_empty_vertex_shader.spv"

  };
  v_shader_ = vulkan::Shader::Create(context, kVertexShader, "main");

  f_shader_ = vulkan::Shader::Create(context, fragment_shader, "main");
}

void renderable::Shader::CleanupCommandBuffers() {
  for (const auto &kCommandBuffer : command_buffers_) {
    rendering_context_->WaitForFence(kCommandBuffer.second.fence);
    rendering_context_->DestroyFence(kCommandBuffer.second.fence);
    rendering_context_->DestroySemaphore(kCommandBuffer.second.semaphore);
    rendering_context_->FreeCommandBuffer(command_pool_, kCommandBuffer.second.command_buffer);
  }
  command_buffers_.clear();
}

void renderable::Shader::SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) {
  {
    uniform_buffer_object_mvp_.screen_width = static_cast<float>(images[0]->GetWidth());
    uniform_buffer_object_mvp_.screen_height = static_cast<float>(images[0]->GetHeight());
  }

  framebuffers_.clear();  // previous images might not be valid anymore
  auto image_format = images[0]->GetPixelFormat();
  if (imgui_wrapper_ != nullptr && imgui_wrapper_->GetImageCount() != images.size()) {
    imgui_wrapper_ = nullptr;
  }
  if (pipeline_ != nullptr && pipeline_->GetDescriptorSetCount() != images.size()) {
    pipeline_ = nullptr;
  }

  if (render_pass_ == nullptr || render_pass_->GetColorAttachmentFormat() != image_format) {
    render_pass_ = std::make_shared<vulkan::RenderPass>(rendering_context_, image_format);
    imgui_wrapper_ = nullptr;
  }
  // imgui
  if (imgui_wrapper_ == nullptr) {
    imgui_wrapper_ =
        std::make_shared<ImguiWrapper>(rendering_context_, render_pass_, images.size(), VK_SAMPLE_COUNT_1_BIT);
  }
  // pipeline
  if (pipeline_ == nullptr) {
    pipeline_ =
        vulkan::RenderingPipeline::Create(rendering_context_, render_pass_, v_shader_, f_shader_, quad_->GetVbl(),
                                          {
                                              .draw_mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                              .cull_mode = VK_CULL_MODE_NONE,
                                              .front_face = VK_FRONT_FACE_CLOCKWISE,
                                              .enable_depth_test = VK_FALSE,
                                              .depth_function = VK_COMPARE_OP_LESS,
                                              .sample_count = VK_SAMPLE_COUNT_1_BIT,
                                          },
                                          images.size());
    pipeline_->SetVertexBuffer(quad_->GetVertexBuffer());
    pipeline_->SetIndexBuffer(quad_->GetIndexBuffer(), quad_->GetIndexBufferDataType());
  }

  for (const auto &kImage : images) {
    std::vector<std::shared_ptr<vulkan::ImageView>> attachments;
    auto image_view = vulkan::ImageView::Create(rendering_context_, kImage);
    attachments.emplace_back(image_view);
    const auto framebuffer = vulkan::Framebuffer::Create(rendering_context_, render_pass_, attachments);
    framebuffers_[kImage] = framebuffer;
  }
  CleanupCommandBuffers();  // recreating is easier than remapping
  uint32_t descriptor_index = 0;
  for (const auto &kImage : images) {
    command_buffers_[kImage] = {
        .semaphore = rendering_context_->CreateSemaphore(),
        .fence = rendering_context_->CreateFence(true),
        .command_buffer = rendering_context_->CreateCommandBuffer(command_pool_),
        .uniform_buffer = vulkan::Buffer::Create(rendering_context_, sizeof(UniformBufferObjectShader),
                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true),
        .descriptor_index = descriptor_index};
    descriptor_index++;
  }
}

VkSemaphore renderable::Shader::Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &semaphore) {
  const auto image_context = command_buffers_[image];
  const auto framebuffer = framebuffers_[image];
  auto *fence = image_context.fence;
  rendering_context_->WaitForFence(fence);
  rendering_context_->ResetFence(fence);

  // prepare pipeline
  { uniform_buffer_object_mvp_.time += 0.003333333333f; }
  {
    memcpy(image_context.uniform_buffer->Map(), &uniform_buffer_object_mvp_, sizeof(uniform_buffer_object_mvp_));
    image_context.uniform_buffer->Unmap();
  }
  pipeline_->SetUniformBuffer(0, image_context.descriptor_index, image_context.uniform_buffer);

  // begin recording commands
  auto *cmd_buffer = image_context.command_buffer;
  VkClearValue clear_value{
      .color =
          {
              .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
          },

  };
  constexpr VkCommandBufferBeginInfo begin_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
  };
  vkBeginCommandBuffer(cmd_buffer, &begin_info);
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
  vkCmdBeginRenderPass(cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
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
  vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);
  vkCmdSetScissor(cmd_buffer, 0, 1, &scissor_rect);

  pipeline_->Draw(cmd_buffer, quad_->GetIndexCount(), 0, image_context.descriptor_index);
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Shader");
  const auto locked_parent = parent_.lock();
  if (locked_parent != nullptr) {
    if (ImGui::Button("<----")) {
      locked_parent->Pop();
    }
  }
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);
  ImGui::End();
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);
  vkCmdEndRenderPass(cmd_buffer);
  vkEndCommandBuffer(cmd_buffer);
  constexpr VkPipelineStageFlags2 wait_stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  rendering_context_->SubmitCommandBuffer(cmd_buffer, semaphore, wait_stage, image_context.semaphore, fence);
  return image_context.semaphore;
}
renderable::Shader::~Shader() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}


#include "shader.hpp"

#include "vulkan/rendering_pipeline.hpp"
#include "vulkan/utils.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

std::shared_ptr<renderable::Shader> renderable::Shader::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                               std::shared_ptr<Menu> parent,
                                                               std::string fragment_shader_path) {
  return std::shared_ptr<Shader>(new Shader(context, parent, fragment_shader_path));
}

renderable::Shader::Shader(std::shared_ptr<vulkan::RenderingContext> context,
                           std::shared_ptr<Menu> parent,
                           std::string fragment_shader_path) :
    rendering_context_(context),
    parent_(parent),
    command_pool_(rendering_context_->CreateCommandPool(VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)) {

  geometry::Point point_0 = {-1.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  geometry::Point point_1 = {1.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  geometry::Point point_2 = {1.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  geometry::Point point_3 = {-1.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};

  quad_ = std::make_shared<geometry::Quad>(rendering_context_, point_0,
                                           point_1, point_2, point_3);

  v_shader_ = vulkan::Shader::Create(context,
                                     SHADER_DIR + std::string("default_empty_vertex_shader.glsl"),
                                     "main",
                                     VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
  f_shader_ = vulkan::Shader::Create(context,
                                     fragment_shader_path,
                                     "main",
                                     VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
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

void renderable::Shader::SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) {
  {
    uniform_buffer_object_mvp_.screen_width = images[0]->GetWidth();
    uniform_buffer_object_mvp_.screen_height = images[0]->GetHeight();
  }

  framebuffers_.clear();// previous images might not be valid anymore
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
  //imgui
  if (imgui_wrapper_ == nullptr) {
    imgui_wrapper_ =
        std::make_shared<ImguiWrapper>(rendering_context_, render_pass_, images.size(), VK_SAMPLE_COUNT_1_BIT);
    imgui_wrapper_->PrepareFonts();
  }
  //pipeline
  if (pipeline_ == nullptr) {
    pipeline_ = vulkan::RenderingPipeline::Create(rendering_context_,
                                                  render_pass_,
                                                  v_shader_,
                                                  f_shader_,
                                                  quad_->GetVbl(),
                                                  {
                                                      .draw_mode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                      .cull_mode = VK_CULL_MODE_NONE,
                                                      .front_face = VkFrontFace::VK_FRONT_FACE_CLOCKWISE,
                                                      .enable_depth_test = VK_FALSE,
                                                      .depth_function = VkCompareOp::VK_COMPARE_OP_LESS,
                                                      .sample_count = VK_SAMPLE_COUNT_1_BIT,
                                                  },
                                                  images.size());
    pipeline_->SetVertexBuffer(quad_->GetVertexBuffer());
    pipeline_->SetIndexBuffer(quad_->GetIndexBuffer(),
                              quad_->GetIndexBufferDataType());
  }

  for (const auto &kImage : images) {
    std::vector<std::shared_ptr<vulkan::ImageView>> attachments;
    auto image_view = vulkan::ImageView::Create(rendering_context_, kImage);
    attachments.emplace_back(image_view);
    auto framebuffer = vulkan::Framebuffer::Create(rendering_context_, render_pass_, attachments);
    framebuffers_[kImage] = framebuffer;
  }
  CleanupCommandBuffers(); //recreating is easier than remapping
  uint32_t descriptor_index = 0;
  for (const auto &kImage : images) {
    auto fence = rendering_context_->CreateFence(true);
    auto semaphore = rendering_context_->CreateSemaphore();
    auto command_buffer = rendering_context_->CreateCommandBuffer(command_pool_);
    auto uniform_buffer = vulkan::Buffer::Create(rendering_context_,
                                                 sizeof(UniformBufferObjectShader),
                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                 true);
    command_buffers_[kImage] = {semaphore, fence, command_buffer, uniform_buffer, descriptor_index++};
  }
}

VkSemaphore renderable::Shader::Render(std::shared_ptr<vulkan::Image> image,
                                       const VkSemaphore &semaphore) {
  auto image_context = command_buffers_[image];
  auto framebuffer = framebuffers_[image];
  auto fence = image_context.fence;
  rendering_context_->WaitForFence(fence);
  rendering_context_->ResetFence(fence);

  //prepare pipeline
  {
    uniform_buffer_object_mvp_.time += 0.003333333333f;
  }
  {
    memcpy(image_context.uniform_buffer->Map(), &uniform_buffer_object_mvp_, sizeof(uniform_buffer_object_mvp_));
    image_context.uniform_buffer->Unmap();
  }
  pipeline_->SetUniformBuffer(0, image_context.descriptor_index, image_context.uniform_buffer);

  //begin recording commands
  auto cmd_buffer = image_context.command_buffer;
  VkClearValue clear_value{
      .color = {
          .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
      },

  };
  VkCommandBufferBeginInfo begin_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags =   VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
  };
  vkBeginCommandBuffer(cmd_buffer, &begin_info);
  VkRenderPassBeginInfo render_pass_begin_info{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = this->render_pass_->GetRenderPass(),
      .framebuffer = framebuffer->GetFramebuffer(),
      .renderArea = {
          .offset = {0, 0},
          .extent = {
              .width = framebuffer->GetWidth(),
              .height = framebuffer->GetHeight(),
          },
      },
      .clearValueCount = 1,
      .pClearValues = &clear_value,
  };
  vkCmdBeginRenderPass(cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  VkViewport viewport{
      .x = 0,
      .y = 0,
      .width = static_cast<float>(framebuffer->GetWidth()),
      .height = static_cast<float>(framebuffer->GetHeight()),
      .minDepth = 0,
      .maxDepth = 1.0,
  };
  VkRect2D scissor_rect{
      .offset = {
          .x = 0,
          .y = 0,
      },
      .extent = {
          .width = framebuffer->GetWidth(),
          .height =  framebuffer->GetHeight(),
      }
  };
  vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);
  vkCmdSetScissor(cmd_buffer, 0, 1, &scissor_rect);

  pipeline_->Draw(cmd_buffer, quad_->GetIndexCount(), 0, image_context.descriptor_index);
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Shader");
  auto locked_parent = parent_.lock();
  if (locked_parent != nullptr) {
    if (ImGui::Button("<----")) {
      locked_parent->Pop();
    }
  }
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);
  vkCmdEndRenderPass(cmd_buffer);
  vkEndCommandBuffer(cmd_buffer);
  VkPipelineStageFlags2 wait_stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  rendering_context_->SubmitCommandBuffer(cmd_buffer,
                                          semaphore,
                                          wait_stage,
                                          image_context.semaphore,
                                          fence);
  return image_context.semaphore;
}
renderable::Shader::~Shader() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}


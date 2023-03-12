#include "model.hpp"

#include "vulkan/rendering_pipeline.hpp"
#include "vulkan/utils.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>

renderable::Model::Model(std::shared_ptr<vulkan::RenderingContext> context,
                         std::shared_ptr<Menu> parent,
                         bool add_depth_attachment,
                         bool use_perspective_projection) :
    rendering_context_(context),
    parent_(parent),
    command_pool_(rendering_context_->CreateCommandPool(VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)),
    add_depth_attachment_(add_depth_attachment),
    use_perspective_projection_(use_perspective_projection) {
}

void renderable::Model::CleanupCommandBuffers() {
  for (const auto &kCommandBuffer : command_buffers_) {
    rendering_context_->WaitForFence(kCommandBuffer.second.fence);
    rendering_context_->DestroyFence(kCommandBuffer.second.fence);
    rendering_context_->DestroySemaphore(kCommandBuffer.second.semaphore);
    rendering_context_->FreeCommandBuffer(command_pool_, kCommandBuffer.second.command_buffer);
  }
  command_buffers_.clear();
}

void renderable::Model::SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) {
  {
    auto width = images[0]->GetWidth();
    auto height = images[0]->GetHeight();
    projection_width_ = 4.0f;
    projection_height_ = static_cast<float>(height * projection_width_) / static_cast<float>(width);
    if (!use_perspective_projection_) {
      uniform_buffer_object_mvp_.proj =
          glm::ortho(-projection_width_, projection_width_, -projection_height_, projection_height_);
    } else {
      uniform_buffer_object_mvp_.proj = glm::perspective(
          glm::radians(45.0f),
          (static_cast<float>(width) / static_cast<float>(height)), 0.1f, 10.0f);
    }
    uniform_buffer_object_mvp_.view = ComputeViewMatrix();
  }

  framebuffers_.clear();// previous images might not be valid anymore
  auto image_format = images[0]->GetPixelFormat();
  if (imgui_wrapper_ != nullptr && imgui_wrapper_->GetImageCount() != images.size()) {
    imgui_wrapper_ = nullptr;
  }
  if (pipeline_ != nullptr && pipeline_->GetDescriptorSetCount() != images.size()) {
    pipeline_ = nullptr;
  }
  auto sample_count =
      std::min(vulkan::GetMaxUsableSampleCount(rendering_context_->GetPhysicalDevice()), VK_SAMPLE_COUNT_8_BIT);
  auto depth_format = VK_FORMAT_UNDEFINED;
  if (add_depth_attachment_) {
    depth_format = vulkan::GetDepthFormat(rendering_context_->GetPhysicalDevice());
  }

  if (render_pass_ == nullptr || render_pass_->GetColorAttachmentFormat() != image_format) {
    render_pass_ = std::make_shared<vulkan::RenderPass>(rendering_context_, image_format, depth_format, sample_count);
    imgui_wrapper_ = nullptr;
  }
  //imgui
  if (imgui_wrapper_ == nullptr) {
    imgui_wrapper_ = std::make_shared<ImguiWrapper>(rendering_context_, render_pass_, images.size(), sample_count);
    imgui_wrapper_->PrepareFonts();
  }
  //pipeline
  if (pipeline_ == nullptr) {
    pipeline_ = CreatePipeline(rendering_context_, render_pass_, images.size(), sample_count);
  }

  for (const auto &kImage : images) {
    std::vector<std::shared_ptr<vulkan::ImageView>> attachments;
    auto image_view = vulkan::ImageView::Create(rendering_context_, kImage);
    if (sample_count == VK_SAMPLE_COUNT_1_BIT) {
      attachments.emplace_back(image_view);
    } else {
      auto color_buffer = vulkan::Image::Create(rendering_context_,
                                                kImage->GetWidth(),
                                                kImage->GetHeight(),
                                                kImage->GetPixelFormat(),
                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                false,
                                                sample_count);
      auto color_image_view = vulkan::ImageView::Create(rendering_context_, color_buffer);
      attachments.emplace_back(color_image_view);
    }
    if (depth_format != VK_FORMAT_UNDEFINED) {
      auto depth_buffer = vulkan::Image::Create(rendering_context_,
                                                kImage->GetWidth(),
                                                kImage->GetHeight(),
                                                depth_format,
                                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                false,
                                                sample_count);
      auto depth_buffer_view = vulkan::ImageView::Create(rendering_context_, depth_buffer);
      attachments.emplace_back(depth_buffer_view);
    }
    if (sample_count != VK_SAMPLE_COUNT_1_BIT) {
      attachments.emplace_back(image_view);
    }
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
                                                 sizeof(UniformBufferObjectMvp),
                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                 true);
    command_buffers_[kImage] = {semaphore, fence, command_buffer, uniform_buffer, descriptor_index++};
  }
}

VkSemaphore renderable::Model::Render(std::shared_ptr<vulkan::Image> image,
                                      const VkSemaphore &semaphore) {
  auto image_context = command_buffers_[image];
  auto framebuffer = framebuffers_[image];
  auto fence = image_context.fence;
  rendering_context_->WaitForFence(fence);
  rendering_context_->ResetFence(fence);

  //prepare pipeline
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_);
    model =
        glm::rotate(model, glm::radians(rotate_.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(rotate_.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(rotate_.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale_);
    uniform_buffer_object_mvp_.model = model;
  }
  {
    memcpy(image_context.uniform_buffer->Map(), &uniform_buffer_object_mvp_, sizeof(uniform_buffer_object_mvp_));
    image_context.uniform_buffer->Unmap();
  }
  pipeline_->SetUniformBuffer(0, image_context.descriptor_index, image_context.uniform_buffer);

  //begin recording commands
  auto cmd_buffer = image_context.command_buffer;
  VkClearValue clear_color_value{
      .color = {
          .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
      },
  };
  std::vector<VkClearValue> clear_values;
  clear_values.emplace_back(clear_color_value);
  if (add_depth_attachment_) {
    VkClearValue depth_clear_value{
        .depthStencil  = {1.0f, 0},
    };
    clear_values.emplace_back(depth_clear_value);
  }
  VkCommandBufferBeginInfo begin_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags =   VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
  };
  vkBeginCommandBuffer(cmd_buffer, &begin_info);
  auto depth_image_view = framebuffer->GetDepthAttachment();
  if (depth_image_view != nullptr) {
    depth_image_view->GetImage()->TransferTo(cmd_buffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }
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
      .clearValueCount = static_cast<uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data(),
  };
  vkCmdBeginRenderPass(cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  VkViewport viewport{
      .x = 0,
      .y = static_cast<float>(framebuffer->GetHeight()),
      .width = static_cast<float>(framebuffer->GetWidth()),
      .height = -static_cast<float>(framebuffer->GetHeight()),
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

  pipeline_->Draw(cmd_buffer, NumOfIndicesToDraw(), 0, image_context.descriptor_index);
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Model");
  ImGui::SliderFloat("translationX", &translation_.x, -projection_width_, projection_width_);
  ImGui::SliderFloat("translationY", &translation_.y, -projection_height_, projection_height_);
  ImGui::SliderFloat2("scale", &scale_.x, 0, 20);
  ImGui::SliderFloat3("rotate", &rotate_.x, 0, 360);
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
renderable::Model::~Model() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}
glm::mat4 renderable::Model::ComputeViewMatrix() {
  if (!use_perspective_projection_) {
    return glm::mat4(1.0f);
  }
  return glm::lookAt(glm::vec3(2.0, 2.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

void renderable::Model::WaitForCommandBuffersToFinish() {
  for (const auto &kCommandBuffer : command_buffers_) {
    rendering_context_->WaitForFence(kCommandBuffer.second.fence);
  }
}

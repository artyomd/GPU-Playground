#include "gltf_model.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include "vulkan/utils.hpp"

std::shared_ptr<renderable::GltfModel> renderable::GltfModel::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent) {
  return std::shared_ptr<GltfModel>(new GltfModel(context, parent));
}

renderable::GltfModel::GltfModel(const std::shared_ptr<vulkan::RenderingContext> &context,
                                 const std::shared_ptr<Menu> &parent)
    : rendering_context_(context),
      parent_(parent),
      command_pool_(rendering_context_->CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)),
      selected_scene_(model_->GetDefaultSceneIndex()) {
  model_ = geometry::GltfModel::Create(rendering_context_, RESOURCE_DIR + std::string("models/Duck.gltf"));
  scenes_names_ = model_->GetScenes();
}

void renderable::GltfModel::CleanupCommandBuffers() {
  for (const auto &kCommandBuffer : command_buffers_) {
    rendering_context_->WaitForFence(kCommandBuffer.second.fence);
    rendering_context_->DestroyFence(kCommandBuffer.second.fence);
    rendering_context_->FreeCommandBuffer(command_pool_, kCommandBuffer.second.command_buffer);
  }
  command_buffers_.clear();
}

void renderable::GltfModel::SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) {
  framebuffers_.clear();  // previous images might not be valid anymore
  auto image_format = images[0]->GetPixelFormat();
  if (imgui_wrapper_ != nullptr && imgui_wrapper_->GetImageCount() != images.size()) {
    imgui_wrapper_ = nullptr;
  }
  auto sample_count = vulkan::GetMaxUsableSampleCount(rendering_context_->GetPhysicalDevice());
  auto depth_format = vulkan::GetDepthFormat(rendering_context_->GetPhysicalDevice());
  if (render_pass_ == nullptr || render_pass_->GetColorAttachmentFormat() != image_format) {
    render_pass_ = std::make_shared<vulkan::RenderPass>(rendering_context_, image_format, depth_format, sample_count);
    imgui_wrapper_ = nullptr;
  }
  // imgui
  if (imgui_wrapper_ == nullptr) {
    imgui_wrapper_ = std::make_shared<ImguiWrapper>(rendering_context_, render_pass_, images.size(), sample_count);
  }
  model_->OnRenderPassChanged(render_pass_, images.size(), sample_count);

  for (const auto &kImage : images) {
    std::vector<std::shared_ptr<vulkan::ImageView>> attachments;
    auto image_view = vulkan::ImageView::Create(rendering_context_, kImage);
    if (sample_count == VK_SAMPLE_COUNT_1_BIT) {
      attachments.emplace_back(image_view);
    } else {
      auto color_buffer =
          vulkan::Image::Create(rendering_context_, kImage->GetWidth(), kImage->GetHeight(), kImage->GetPixelFormat(),
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, false, sample_count);
      auto color_image_view = vulkan::ImageView::Create(rendering_context_, color_buffer);
      attachments.emplace_back(color_image_view);
    }
    if (depth_format != VK_FORMAT_UNDEFINED) {
      auto depth_buffer =
          vulkan::Image::Create(rendering_context_, kImage->GetWidth(), kImage->GetHeight(), depth_format,
                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false, sample_count);
      auto depth_buffer_view = vulkan::ImageView::Create(rendering_context_, depth_buffer);
      attachments.emplace_back(depth_buffer_view);
    }
    if (sample_count != VK_SAMPLE_COUNT_1_BIT) {
      attachments.emplace_back(image_view);
    }
    const auto framebuffer = vulkan::Framebuffer::Create(rendering_context_, render_pass_, attachments);
    framebuffers_[kImage] = framebuffer;
  }
  CleanupCommandBuffers();  // recreating is easier than remapping
  uint32_t descriptor_index = 0;
  for (const auto &kImage : images) {
    command_buffers_[kImage] = {.fence = rendering_context_->CreateFence(true),
                                .command_buffer = rendering_context_->CreateCommandBuffer(command_pool_),
                                .descriptor_index = descriptor_index++};
  }
}

void renderable::GltfModel::Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &waitSemaphore,
                                   const VkSemaphore &signalSemaphore) {
  if (current_scene_ != selected_scene_) {
    rendering_context_->WaitForGraphicsQueueIdle();
    model_->LoadScene(selected_scene_);
    current_scene_ = selected_scene_;
  }
  const auto image_context = command_buffers_[image];
  const auto framebuffer = framebuffers_[image];
  auto *fence = image_context.fence;
  rendering_context_->WaitForFence(fence);
  rendering_context_->ResetFence(fence);

  // begin recording commands
  auto *cmd_buffer = image_context.command_buffer;
  constexpr VkClearValue clear_color_value{
      .color =
          {
              .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
          },
  };
  constexpr VkClearValue depth_clear_value{
      .depthStencil = {1.0f, 0},
  };
  std::array clear_values = {clear_color_value, depth_clear_value};
  constexpr VkCommandBufferBeginInfo begin_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
  };
  vkBeginCommandBuffer(cmd_buffer, &begin_info);
  const auto depth_image_view = framebuffer->GetDepthAttachment();
  if (depth_image_view != nullptr) {
    depth_image_view->GetImage()->TransferTo(cmd_buffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }
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
      .clearValueCount = static_cast<uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data(),
  };
  vkCmdBeginRenderPass(cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  const VkViewport viewport{
      .x = 0,
      .y = static_cast<float>(framebuffer->GetHeight()),
      .width = static_cast<float>(framebuffer->GetWidth()),
      .height = -static_cast<float>(framebuffer->GetHeight()),
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

  model_->Render(cmd_buffer, image_context.descriptor_index);

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Gltf model");
  ImGui::Text("Scenes:");
  if (ImGui::BeginListBox("##Scenes", ImVec2(-FLT_MIN, std::min(static_cast<float>(scenes_names_.size()), 7.0f) *
                                                           ImGui::GetTextLineHeightWithSpacing()))) {
    for (size_t n = 0; n < scenes_names_.size(); n++) {
      const bool kIsSelected = (selected_scene_ == static_cast<int>(n));
      if (ImGui::Selectable(scenes_names_[n].c_str(), kIsSelected)) {
        selected_scene_ = static_cast<int>(n);
      }
      if (kIsSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndListBox();
  }

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
  rendering_context_->SubmitCommandBuffer(cmd_buffer, waitSemaphore, wait_stage, signalSemaphore, fence);
}
renderable::GltfModel::~GltfModel() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}

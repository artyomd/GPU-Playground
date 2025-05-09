#include "default_menu.hpp"

#include <imgui.h>
#include <ranges>

#include "vulkan/utils.hpp"

std::shared_ptr<renderable::DefaultMenu> renderable::DefaultMenu::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent) {
  return std::shared_ptr<DefaultMenu>(new DefaultMenu(context, parent));
}

renderable::DefaultMenu::DefaultMenu(const std::shared_ptr<vulkan::RenderingContext> &context,
                                     const std::shared_ptr<Menu> &parent)
    : rendering_context_(context), parent_(parent) {
  if (parent_.expired()) {
    throw std::runtime_error("parent menu can not be nullptr");
  }
  command_pool_ = rendering_context_->CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void renderable::DefaultMenu::CleanupCommandBuffers() {
  for (auto [fence, command_buffer] : command_buffers_ | std::views::values) {
    rendering_context_->WaitForFence(fence);
    rendering_context_->DestroyFence(fence);
    rendering_context_->FreeCommandBuffer(command_pool_, command_buffer);
  }
  command_buffers_.clear();
}

void renderable::DefaultMenu::SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) {
  framebuffers_.clear();  // previous images might not be valid anymore
  auto image_format = images[0]->GetPixelFormat();
  if (imgui_wrapper_ != nullptr && imgui_wrapper_->GetImageCount() != images.size()) {
    imgui_wrapper_ = nullptr;
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
  for (const auto &kImage : images) {
    std::vector<std::shared_ptr<vulkan::ImageView>> attachments;
    auto image_view = vulkan::ImageView::Create(rendering_context_, kImage);
    attachments.emplace_back(image_view);
    const auto framebuffer = vulkan::Framebuffer::Create(rendering_context_, render_pass_, attachments);
    framebuffers_[kImage] = framebuffer;
  }
  CleanupCommandBuffers();  // recreating is easier than remapping
  for (const auto &kImage : images) {
    command_buffers_[kImage] = {.fence = rendering_context_->CreateFence(true),
                                .command_buffer = rendering_context_->CreateCommandBuffer(command_pool_)};
  }
}

void renderable::DefaultMenu::Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &waitSemaphore,
                                     const VkSemaphore &signalSemaphore) {
  // render menu
  auto [fence, command_buffer] = command_buffers_[image];
  const auto framebuffer = framebuffers_[image];
  rendering_context_->WaitForFence(fence);
  rendering_context_->ResetFence(fence);

  // begin recording commands
  auto *cmd_buffer = command_buffer;
  VkClearValue clear_value{
      .color =
          {
              .float32 = {0.0, 0.0, 0.0, 1.0},
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

  ImguiWrapper::BeginFrame();
  ImguiWrapper::ShowFPSOverlay();
  ImGui::Begin("Menu");
  {
    const auto locked_parent = parent_.lock();
    for (const auto &kItem : locked_parent->EntryNames()) {
      if (ImGui::Button(kItem.c_str())) {
        locked_parent->Select(kItem);
      }
    }
    if (ImGui::Button("<----")) {
      locked_parent->PopSelf();
    }
  }
  ImGui::End();
  ImguiWrapper::EndFrameAndDraw(cmd_buffer);
  vkCmdEndRenderPass(cmd_buffer);
  vkEndCommandBuffer(cmd_buffer);
  constexpr VkPipelineStageFlags2 wait_stage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  rendering_context_->SubmitCommandBuffer(cmd_buffer, waitSemaphore, wait_stage, signalSemaphore, fence);
}

renderable::DefaultMenu::~DefaultMenu() {
  CleanupCommandBuffers();
  rendering_context_->DestroyCommandPool(command_pool_);
}

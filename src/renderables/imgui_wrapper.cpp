#include "imgui_wrapper.hpp"

#include "vulkan/utils.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

renderable::ImguiWrapper::ImguiWrapper(std::shared_ptr<vulkan::RenderingContext> context,
                                       std::shared_ptr<vulkan::RenderPass> render_pass,
                                       uint32_t image_count,
                                       VkSampleCountFlagBits sample_count)
    : rendering_context_(context),
      render_pass_(render_pass),
      image_count_(image_count) {
  auto *vulkan_instance = rendering_context_->GetInstance();
  ImGui_ImplVulkan_LoadFunctions([](const char *function_name, void *vulkan_instance) {
    return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance *>(vulkan_instance)), function_name);
  }, &vulkan_instance);
  ImGui_ImplVulkan_InitInfo init_info = {
      .Instance = rendering_context_->GetInstance(),
      .PhysicalDevice = rendering_context_->GetPhysicalDevice(),
      .Device = rendering_context_->GetDevice(),
      .QueueFamily = 0,
      .Queue = rendering_context_->GetGraphicsQueue(),
      .PipelineCache = VK_NULL_HANDLE,
      .DescriptorPool = rendering_context_->GetDescriptorPool(),
      .Subpass = 0,
      .MinImageCount = image_count_,
      .ImageCount = image_count_,
      .MSAASamples = sample_count,
      .Allocator = nullptr,
      .CheckVkResultFn = [](VkResult result) {
        VK_CALL(result);
      },
  };
  ImGui_ImplVulkan_Init(&init_info, render_pass_->GetRenderPass());
  ImGui::StyleColorsDark();

}

uint32_t renderable::ImguiWrapper::GetImageCount() const {
  return image_count_;
}

void renderable::ImguiWrapper::PrepareFonts() {
  auto tmp_command_pool = rendering_context_->CreateCommandPool();
  auto cmd_buffer = rendering_context_->CreateCommandBuffer(tmp_command_pool);
  VkCommandBufferBeginInfo begin_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags =   VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
  };
  vkBeginCommandBuffer(cmd_buffer, &begin_info);
  ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
  vkEndCommandBuffer(cmd_buffer);
  auto fence = rendering_context_->CreateFence();
  rendering_context_->SubmitCommandBuffer(cmd_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
  rendering_context_->WaitForFence(fence);
  rendering_context_->DestroyFence(fence);
  rendering_context_->FreeCommandBuffer(tmp_command_pool, cmd_buffer);
  rendering_context_->DestroyCommandPool(tmp_command_pool);
}

renderable::ImguiWrapper::~ImguiWrapper() {
  ImGui_ImplVulkan_Shutdown();
}


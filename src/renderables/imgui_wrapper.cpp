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
      .RenderPass = render_pass_->GetRenderPass(),
      .Subpass = 0,
      .MinImageCount = image_count_,
      .ImageCount = image_count_,
      .MSAASamples = sample_count,
      .Allocator = nullptr,
      .CheckVkResultFn = [](VkResult result) {
        VK_CALL(result);
      },
  };
  ImGui_ImplVulkan_Init(&init_info);
  ImGui::StyleColorsDark();
}

uint32_t renderable::ImguiWrapper::GetImageCount() const {
  return image_count_;
}

renderable::ImguiWrapper::~ImguiWrapper() {
  ImGui_ImplVulkan_Shutdown();
}


#include "imgui_wrapper.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "vulkan/utils.hpp"

renderable::ImguiWrapper::ImguiWrapper(const std::shared_ptr<vulkan::RenderingContext>& context,
                                       const std::shared_ptr<vulkan::RenderPass>& render_pass,
                                       const uint32_t& image_count, const VkSampleCountFlagBits& sample_count)
    : rendering_context_(context), render_pass_(render_pass), image_count_(image_count) {
  auto* vulkan_instance = rendering_context_->GetInstance();
  ImGui_ImplVulkan_LoadFunctions(
      [](const char* function_name, void* vulkan_instance) {
        return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name);
      },
      &vulkan_instance);
  ImGui_ImplVulkan_InitInfo init_info = {
      .Instance = rendering_context_->GetInstance(),
      .PhysicalDevice = rendering_context_->GetPhysicalDevice(),
      .Device = rendering_context_->GetDevice(),
      .QueueFamily = 0,
      .Queue = rendering_context_->GetGraphicsQueue(),
      .DescriptorPool = rendering_context_->GetDescriptorPool(),
      .RenderPass = render_pass_->GetRenderPass(),
      .MinImageCount = image_count_,
      .ImageCount = image_count_,
      .MSAASamples = sample_count,
      .PipelineCache = VK_NULL_HANDLE,
      .Subpass = 0,
      .Allocator = nullptr,
      .CheckVkResultFn = [](const VkResult result) { VK_CALL(result); },
  };
  ImGui_ImplVulkan_Init(&init_info);
  ImGui::StyleColorsDark();
}

uint32_t renderable::ImguiWrapper::GetImageCount() const { return image_count_; }

renderable::ImguiWrapper::~ImguiWrapper() { ImGui_ImplVulkan_Shutdown(); }

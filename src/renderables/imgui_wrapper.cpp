#include "imgui_wrapper.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "vulkan/utils.hpp"

renderable::ImguiWrapper::ImguiWrapper(const std::shared_ptr<vulkan::RenderingContext>& context,
                                       const std::shared_ptr<vulkan::RenderPass>& render_pass,
                                       const uint32_t& image_count, const VkSampleCountFlagBits& sample_count)
    : rendering_context_(context), render_pass_(render_pass), image_count_(image_count) {
  auto* vulkan_instance = rendering_context_->GetInstance();
  uint32_t instance_version = VK_API_VERSION_1_0;
  VK_CALL(vkEnumerateInstanceVersion(&instance_version));
  ImGui_ImplVulkan_LoadFunctions(
      instance_version,
      [](const char* function_name, void* vulkan_instance) {
        return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name);
      },
      &vulkan_instance);
  ImGui_ImplVulkan_InitInfo init_info = {
      .ApiVersion = instance_version,
      .Instance = rendering_context_->GetInstance(),
      .PhysicalDevice = rendering_context_->GetPhysicalDevice(),
      .Device = rendering_context_->GetDevice(),
      .QueueFamily = 0,
      .Queue = rendering_context_->GetQueue(),
      .DescriptorPool = rendering_context_->GetDescriptorPool(),
      .DescriptorPoolSize = 0,
      .MinImageCount = image_count_,
      .ImageCount = image_count_,
      .PipelineCache = VK_NULL_HANDLE,
      .RenderPass = render_pass_->GetRenderPass(),
      .Subpass = 0,
      .MSAASamples = sample_count,
      .Allocator = nullptr,
      .CheckVkResultFn = [](const VkResult result) { VK_CALL(result); },
  };
  ImGui_ImplVulkan_Init(&init_info);
  ImGui::StyleColorsDark();
}

uint32_t renderable::ImguiWrapper::GetImageCount() const { return image_count_; }

void renderable::ImguiWrapper::ShowFPSOverlay() {
  const ImGuiIO& io = ImGui::GetIO();
  constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
  constexpr float padding = 10.0f;
  const ImVec2 work_pos = ImGui::GetMainViewport()->WorkPos;
  ImGui::SetNextWindowPos(ImVec2(work_pos.x + padding, work_pos.y + padding), ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::Begin("FPS Overlay", nullptr, window_flags);
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  ImGui::End();
}
void renderable::ImguiWrapper::BeginFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}
void renderable::ImguiWrapper::EndFrameAndDraw(const VkCommandBuffer& cmd_buffer) {
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);
}

renderable::ImguiWrapper::~ImguiWrapper() { ImGui_ImplVulkan_Shutdown(); }

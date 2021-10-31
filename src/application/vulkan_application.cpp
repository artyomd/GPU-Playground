//
// Created by artyomd on 12/7/19.
//

#include "src/application/vulkan_application.hpp"

#include <set>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <array>

#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace {
VkResult CreateDebugUtilsMessengerExt(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger
) {
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
  );
  if (func != nullptr) {
    return func(instance, p_create_info, p_allocator, p_debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerExt(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks *p_allocator
) {
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
  );
  if (func != nullptr) {
    func(instance, debug_messenger, p_allocator);
  }
}

void CheckVkResult(VkResult result) {
  if (result == VK_SUCCESS) {
    return;
  }
  throw std::runtime_error("imgui vk error");
}

std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
  return available_extensions;
}
}

application::VulkanApplication::VulkanApplication()
    : GlfwApplication() {}

void application::VulkanApplication::SetupWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void application::VulkanApplication::OnWindowSizeChanged(int width, int height) {
  this->framebuffer_resized_ = true;
  this->framebuffer_empty_ = width == 0 || height == 0;
}

void application::VulkanApplication::InitContext() {
  GlfwApplication::InitContext();
  CreateInstance();
  SetupDebugMessenger();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
  CreateCommandPool();
  auto context = std::make_shared<api::vulkan::VulkanRenderingContext>(
      physical_device_,
      device_,
      graphics_queue_,
      graphics_command_pool_,
      max_frames_in_flight_);
  SetContext(context);
  CreateSwapChain();
  CreateImageViews();
  CreateColorResources();
  CreateDepthResources();
  CreateFrameBuffers();
  CreateCommandBuffers();

  CreateSyncObjects();

  InitImGui();
}

void application::VulkanApplication::RecreateSwapChain() {
  while (framebuffer_empty_) {
    glfwWaitEvents();
  }
  GetVulkanContext()->WaitForGpuIdle();

  CleanupSwapChain();

  CreateSwapChain();
  CreateImageViews();
  CreateColorResources();
  CreateDepthResources();
  CreateFrameBuffers();
  CreateCommandBuffers();
}

void application::VulkanApplication::CreateInstance() {
  if (enable_validation_layers_ && !CheckValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "GPU Playground";
  app_info.applicationVersion = 0U;
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = 0U;
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  auto extensions = GetRequiredExtensions();
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  if (enable_validation_layers_) {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
    create_info.ppEnabledLayerNames = validation_layers_.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&create_info, nullptr, &vulkan_instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

bool application::VulkanApplication::CheckValidationLayerSupport() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char *layer_name: validation_layers_) {
    for (const auto &layer_properties: available_layers) {
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        return true;
      }
    }
  }
  return false;
}

std::vector<const char *> application::VulkanApplication::GetRequiredExtensions() const {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
  if (enable_validation_layers_) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  return extensions;
}

void application::VulkanApplication::SetupDebugMessenger() {
  if (!enable_validation_layers_) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;
  create_info.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                   VkDebugUtilsMessageTypeFlagsEXT,
                                   const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                                   void *) -> VKAPI_ATTR VkBool32 VKAPI_CALL {
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
      return VK_FALSE;
    }
    if ((message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        | (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
      std::cerr << "validation layer: " << p_callback_data->pMessage << std::endl;
    } else {
      std::cout << "validation layer: " << p_callback_data->pMessage << std::endl;
    }
    return VK_FALSE;
  };
  if (CreateDebugUtilsMessengerExt(vulkan_instance_, &create_info, nullptr, &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

void application::VulkanApplication::CreateSurface() {
  if (glfwCreateWindowSurface(vulkan_instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void application::VulkanApplication::PickPhysicalDevice() {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vulkan_instance_, &device_count, nullptr);
  if (device_count == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(vulkan_instance_, &device_count, devices.data());
  for (const auto &device: devices) {
    if (IsDeviceSuitable(device)) {
      physical_device_ = device;
      break;
    }
  }
  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool application::VulkanApplication::IsDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = FindQueueFamilies(device);
  bool extensions_supported = CheckDeviceExtensionSupport(device);
  bool swap_chain_adequate;
  if (extensions_supported) {
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
    swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
  }
  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);
  return indices.IsComplete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
}

application::VulkanApplication::QueueFamilyIndices application::VulkanApplication::FindQueueFamilies(
    VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  unsigned int i = 0;
  for (const auto &queue_family: queue_families) {
    if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
    if (queue_family.queueCount > 0 && present_support) {
      indices.present_family = i;
    }
    if (indices.IsComplete()) {
      break;
    }
    i++;
  }
  return indices;
}

bool application::VulkanApplication::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
  auto available_extensions = GetDeviceExtensions(device);
  std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());
  for (const auto &extension: available_extensions) {
    required_extensions.erase(extension.extensionName);
  }
  return required_extensions.empty();
}

application::VulkanApplication::SwapChainSupportDetails application::VulkanApplication::QuerySwapChainSupport(
    VkPhysicalDevice device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
  }
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes.data());
  }
  return details;
}

void application::VulkanApplication::CreateLogicalDevice() {
  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {
      indices.graphics_family.value(),
      indices.present_family.value()
  };
  float queue_priority = 1.0f;
  for (uint32_t queue_family: unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features = {};

  auto device_extensions = GetDeviceExtensions(physical_device_);
  std::vector<const char *> extensions_to_enable(device_extensions_.begin(), device_extensions_.end());
  for (auto ext: device_extensions) {
    if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
      extensions_to_enable.emplace_back("VK_KHR_portability_subset");
    }
  }

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos = queue_create_infos.data();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions_to_enable.size());
  create_info.ppEnabledExtensionNames = extensions_to_enable.data();

  if (enable_validation_layers_) {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
    create_info.ppEnabledLayerNames = validation_layers_.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
  vkGetDeviceQueue(device_, indices.present_family.value(), 0, &present_queue_);
  vkGetDeviceQueue(device_, indices.graphics_family.value(), 0, &graphics_queue_);
}

void application::VulkanApplication::CreateCommandPool() {
  QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  if (vkCreateCommandPool(device_, &pool_info, nullptr, &graphics_command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void application::VulkanApplication::CreateSwapChain() {
  SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(physical_device_);
  VkSurfaceFormatKHR
      surface_format =
      ChooseSwapSurfaceFormat(swap_chain_support.formats, GetVulkanContext()->GetColorAttachmentFormat());
  VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
  swap_chain_extent_ = ChooseSwapExtent(swap_chain_support.capabilities);
  swap_chain_image_format_ = surface_format.format;
  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }
  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface_;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = swap_chain_extent_;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
  uint32_t queue_family_indices[] = {indices.graphics_family.value(),
                                     indices.present_family.value()};
  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }
  create_info.preTransform = swap_chain_support.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;
  if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }
  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, swap_chain_images_.data());
}

VkSurfaceFormatKHR
application::VulkanApplication::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &available_formats,
    VkFormat desired_format) {
  for (const auto &available_format: available_formats) {
    if (available_format.format == desired_format &&
        available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_format;
    }
  }
  return available_formats[0];
}

VkPresentModeKHR
application::VulkanApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes) {
  for (const auto &available_present_mode: available_present_modes) {
    if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return available_present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D application::VulkanApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);
    VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    actual_extent.width = std::max(capabilities.minImageExtent.width,
                                   std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(capabilities.minImageExtent.height,
                                    std::min(capabilities.maxImageExtent.height, actual_extent.height));
    return actual_extent;
  }
}

void application::VulkanApplication::CreateImageViews() {
  swap_chain_image_views_.resize(swap_chain_images_.size());
  for (size_t i = 0; i < swap_chain_images_.size(); i++) {
    GetVulkanContext()->CreateImageView(
        swap_chain_images_[i],
        swap_chain_image_format_,
        VK_IMAGE_ASPECT_COLOR_BIT,
        &swap_chain_image_views_[i]);
  }
}

void application::VulkanApplication::CreateColorResources() {
  VkFormat color_format = swap_chain_image_format_;
  auto context = GetVulkanContext();
  context->CreateImage(swap_chain_extent_.width, swap_chain_extent_.height,
                       context->GetRecommendedMsaaSamples(), color_format,
                       VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       &color_image_,
                       &color_image_memory_);
  context->CreateImageView(color_image_, color_format, VK_IMAGE_ASPECT_COLOR_BIT, &color_image_view_);
  context->TransitionImageLayout(color_image_,
                                 VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void application::VulkanApplication::CreateDepthResources() {
  auto context = GetVulkanContext();
  VkFormat depth_format = context->GetDepthAttachmentFormat();
  context->CreateImage(swap_chain_extent_.width, swap_chain_extent_.height,
                       context->GetRecommendedMsaaSamples(),
                       depth_format,
                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                       &depth_image_,
                       &depth_image_memory_);
  context->CreateImageView(depth_image_, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, &depth_image_view_);
  context->TransitionImageLayout(depth_image_,
                                 VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void application::VulkanApplication::CreateFrameBuffers() {
  swap_chain_frame_buffers_.resize(swap_chain_image_views_.size());
  for (size_t i = 0; i < swap_chain_image_views_.size(); i++) {
    std::array<VkImageView, 3> attachments = {
        color_image_view_,
        depth_image_view_,
        swap_chain_image_views_[i]
    };

    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = GetVulkanContext()->GetRenderPass();
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;
    if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &swap_chain_frame_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void application::VulkanApplication::CreateCommandBuffers() {
  graphics_command_buffers_.resize(swap_chain_frame_buffers_.size());
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = graphics_command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = static_cast<uint32_t>(graphics_command_buffers_.size());
  if (vkAllocateCommandBuffers(device_, &alloc_info, graphics_command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void application::VulkanApplication::CreateSyncObjects() {
  image_available_semaphores_.resize(max_frames_in_flight_);
  render_finished_semaphores_.resize(max_frames_in_flight_);
  in_flight_fences_.resize(max_frames_in_flight_);
  images_in_flight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);
  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkFenceCreateInfo fence_info = {};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (size_t i = 0; i < max_frames_in_flight_; i++) {
    if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device_, &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
        vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create sync objects for a frame!");
    }
  }
}

void application::VulkanApplication::InitImGui() {
  auto context = GetVulkanContext();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForVulkan(window_, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vulkan_instance_;
  init_info.PhysicalDevice = physical_device_;
  init_info.MSAASamples = context->GetRecommendedMsaaSamples();
  init_info.Device = device_;
  init_info.Queue = graphics_queue_;
  init_info.Allocator = nullptr;
  init_info.MinImageCount = 2;
  init_info.DescriptorPool = context->GetDescriptorPool();
  init_info.ImageCount = max_frames_in_flight_;
  init_info.CheckVkResultFn = CheckVkResult;
  ImGui_ImplVulkan_Init(&init_info, context->GetRenderPass());

  VkCommandBuffer buffer = context->BeginSingleTimeCommands(graphics_command_pool_);
  ImGui_ImplVulkan_CreateFontsTexture(buffer);
  context->EndSingleTimeCommands(graphics_queue_, graphics_command_pool_, buffer);
  ImGui_ImplVulkan_DestroyFontUploadObjects();

  ImGui::StyleColorsDark();
}

bool application::VulkanApplication::PrepareFrame() {
  if (framebuffer_resized_) {
    RecreateSwapChain();
    framebuffer_resized_ = false;
  }
  vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

  VkResult result = vkAcquireNextImageKHR(device_, swap_chain_, UINT64_MAX, image_available_semaphores_[current_frame_],
                                          VK_NULL_HANDLE, &current_image_);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    framebuffer_resized_ = true;
    return false;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  if (images_in_flight_[current_image_] != VK_NULL_HANDLE) {
    vkWaitForFences(device_, 1, &images_in_flight_[current_image_], VK_TRUE, UINT64_MAX);
  }
  images_in_flight_[current_image_] = in_flight_fences_[current_frame_];

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(graphics_command_buffers_[current_frame_], &begin_info);

  auto context = GetVulkanContext();
  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = context->GetRenderPass();
  render_pass_info.framebuffer = swap_chain_frame_buffers_[current_image_];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swap_chain_extent_;

  std::array<VkClearValue, 2> clear_values = {};
  clear_values[0].color = {{0.0, 0.0, 0.0, 1.0}};
  clear_values[1].depthStencil = {1.0f, 0};
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();
  vkCmdBeginRenderPass(graphics_command_buffers_[current_frame_], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
  context->SetCurrentCommandBuffer(graphics_command_buffers_[current_frame_]);
  return true;
}

void application::VulkanApplication::PrepareImGuiFrame() {
  ImGui_ImplVulkan_NewFrame();
}

void application::VulkanApplication::RenderImGui() {
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), graphics_command_buffers_[current_frame_]);
}

void application::VulkanApplication::RenderFrame() {
  vkCmdEndRenderPass(graphics_command_buffers_[current_frame_]);
  vkEndCommandBuffer(graphics_command_buffers_[current_frame_]);
  GetVulkanContext()->SetCurrentCommandBuffer(nullptr);

  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &image_available_semaphores_[current_frame_];
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &graphics_command_buffers_[current_frame_];
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &render_finished_semaphores_[current_frame_];

  vkResetFences(device_, 1, &in_flight_fences_[current_frame_]);
  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &render_finished_semaphores_[current_frame_];
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swap_chain_;
  present_info.pImageIndices = &current_image_;

  VkResult result = vkQueuePresentKHR(present_queue_, &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    framebuffer_resized_ = true;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  current_frame_ = (current_frame_ + 1) % max_frames_in_flight_;
  GetVulkanContext()->SetCurrentImageIndex(current_frame_);
}

void application::VulkanApplication::DestroyImGui() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void application::VulkanApplication::CleanupSwapChain() {
  vkDestroyImageView(device_, color_image_view_, nullptr);
  vkDestroyImage(device_, color_image_, nullptr);
  vkFreeMemory(device_, color_image_memory_, nullptr);
  vkDestroyImageView(device_, depth_image_view_, nullptr);
  vkDestroyImage(device_, depth_image_, nullptr);
  vkFreeMemory(device_, depth_image_memory_, nullptr);
  for (auto framebuffer: swap_chain_frame_buffers_) {
    vkDestroyFramebuffer(device_, framebuffer, nullptr);
  }
  vkFreeCommandBuffers(device_,
                       graphics_command_pool_,
                       static_cast<uint32_t>(graphics_command_buffers_.size()),
                       graphics_command_buffers_.data());
  for (auto image_view: swap_chain_image_views_) {
    vkDestroyImageView(device_, image_view, nullptr);
  }
  vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
}

void application::VulkanApplication::DestroyContext() {
  SetContext(nullptr);
  DestroyImGui();
  CleanupSwapChain();
  for (size_t i = 0; i < max_frames_in_flight_; i++) {
    vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
    vkDestroyFence(device_, in_flight_fences_[i], nullptr);
  }
  vkDestroyCommandPool(device_, graphics_command_pool_, nullptr);
  vkDestroyDevice(device_, nullptr);
  vkDestroySurfaceKHR(vulkan_instance_, surface_, nullptr);
  if (enable_validation_layers_) {
    DestroyDebugUtilsMessengerExt(vulkan_instance_, debug_messenger_, nullptr);
  }
  vkDestroyInstance(vulkan_instance_, nullptr);
  GlfwApplication::DestroyContext();
}

std::shared_ptr<api::vulkan::VulkanRenderingContext> application::VulkanApplication::GetVulkanContext() {
  return std::dynamic_pointer_cast<api::vulkan::VulkanRenderingContext>(GetContext());
}

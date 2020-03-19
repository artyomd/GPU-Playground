//
// Created by artyomd on 12/7/19.
//

#include <vendor/imgui/imgui.h>
#include <vendor/imgui/imgui_impl_glfw.h>
#include <vendor/imgui/imgui_impl_vulkan.h>
#include "VulkanApplication.h"
#include "api/vulkan/VkRenderingContext.h"
#include <set>

namespace application {

VkResult CreateDebugUtilsMessengerExt(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_debug_messenger
) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func!=nullptr) {
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
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func!=nullptr) {
    func(instance, debug_messenger, p_allocator);
  }
}

void CheckVkResult(VkResult result) {
  if (result==VK_SUCCESS) {
    return;
  }
  throw std::runtime_error("imgui vk error");
}

void VulkanApplication::SetupWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanApplication::OnWindowSizeChanged() {
  this->framebuffer_resized_ = true;
}

void VulkanApplication::InitContext() {
  CreateInstance();
  SetupDebugMessenger();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
  CreateDescriptorPool();
  context_ = new api::VkRenderingContext(
      &physical_device_,
      &device_,
      &graphics_command_pool_,
      &graphics_queue_,
      &descriptor_pool_,
      max_frames_in_flight_);
  renderer_->SetContext(context_);
  CreateCommandPool();

  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateFrameBuffers();
  CreateCommandBuffers();

  CreateSyncObjects();
  PrepareTestMenu();
}

void VulkanApplication::RecreateSwapChain() {
  while (window_width_==0 || window_height_==0) {
    glfwGetFramebufferSize(window_, &window_width_, &window_height_);
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(device_);

  CleanupSwapChain();

  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateFrameBuffers();
  CreateCommandBuffers();
}

void VulkanApplication::CreateInstance() {
  if (enable_validation_layers_ && !CheckValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "GPU Playground";
  app_info.applicationVersion = 0U;
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = 0U;
  app_info.apiVersion = VK_API_VERSION_1_1;

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

  if (vkCreateInstance(&create_info, nullptr, &vulkan_instance_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

bool VulkanApplication::CheckValidationLayerSupport() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char *layer_name : validation_layers_) {
    for (const auto &layer_properties : available_layers) {
      if (strcmp(layer_name, layer_properties.layerName)==0) {
        return true;
      }
    }
  }
  return false;
}

std::vector<const char *> VulkanApplication::GetRequiredExtensions() {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
  if (enable_validation_layers_) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  return extensions;
}

void VulkanApplication::SetupDebugMessenger() {
  if (!enable_validation_layers_) {
    return;
  }
  VkDebugUtilsMessengerCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;
  create_info.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                   VkDebugUtilsMessageTypeFlagsEXT message_type,
                                   const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                                   void *p_user_data) -> VKAPI_ATTR VkBool32 VKAPI_CALL {
    std::cerr << "validation layer: " << p_callback_data->pMessage << std::endl;
    return VK_FALSE;
  };
  if (CreateDebugUtilsMessengerExt(vulkan_instance_, &create_info, nullptr, &debug_messenger_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

void VulkanApplication::CreateSurface() {
  if (glfwCreateWindowSurface(vulkan_instance_, window_, nullptr, &surface_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void VulkanApplication::PickPhysicalDevice() {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vulkan_instance_, &device_count, nullptr);
  if (device_count==0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(vulkan_instance_, &device_count, devices.data());
  for (const auto &device : devices) {
    if (IsDeviceSuitable(device)) {
      physical_device_ = device;
      break;
    }
  }
  if (physical_device_==VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool VulkanApplication::IsDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = FindQueueFamilies(device);
  bool extensions_supported = CheckDeviceExtensionSupport(device);
  bool swap_chain_adequate = false;
  if (extensions_supported) {
    SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
    swap_chain_adequate = !swap_chain_support.formats_.empty() && !swap_chain_support.present_modes_.empty();
  }
  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(device, &supported_features);
  return indices.IsComplete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy;
}

VulkanApplication::QueueFamilyIndices VulkanApplication::FindQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  int i = 0;
  for (const auto &queue_family :queue_families) {
    if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family_ = i;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
    if (queue_family.queueCount > 0 && present_support) {
      indices.present_family_ = i;
    }
    if (indices.IsComplete()) {
      break;
    }
    i++;
  }
  return indices;
}

bool VulkanApplication::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
  std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());
  for (const auto &extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }
  return required_extensions.empty();
}

VulkanApplication::SwapChainSupportDetails VulkanApplication::QuerySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities_);
  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
  if (format_count!=0) {
    details.formats_.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats_.data());
  }
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);
  if (present_mode_count!=0) {
    details.present_modes_.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes_.data());
  }
  return details;
}

void VulkanApplication::CreateLogicalDevice() {
  QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {
      indices.graphics_family_.value(),
      indices.present_family_.value()
  };
  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features = {};

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos = queue_create_infos.data();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions_.size());
  create_info.ppEnabledExtensionNames = device_extensions_.data();

  if (enable_validation_layers_) {
    create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
    create_info.ppEnabledLayerNames = validation_layers_.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
  vkGetDeviceQueue(device_, indices.present_family_.value(), 0, &present_queue_);
  vkGetDeviceQueue(device_, indices.graphics_family_.value(), 0, &graphics_queue_);
}

void VulkanApplication::CreateDescriptorPool() {
  VkDescriptorPoolSize pool_sizes[] =
      {
          {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
          {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
          {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
          {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
          {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
          {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
          {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
      };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000*IM_ARRAYSIZE(pool_sizes);
  pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &descriptor_pool_)!=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

void VulkanApplication::CreateCommandPool() {
  QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device_);
  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = queue_family_indices.graphics_family_.value();
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  if (vkCreateCommandPool(device_, &pool_info, nullptr, &graphics_command_pool_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void VulkanApplication::CreateSwapChain() {
  SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(physical_device_);
  VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats_);
  VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes_);
  swap_chain_extent_ = ChooseSwapExtent(swap_chain_support.capabilities_);
  swap_chain_image_format_ = surface_format.format;
  uint32_t image_count = swap_chain_support.capabilities_.minImageCount + 1;
  if (swap_chain_support.capabilities_.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities_.maxImageCount) {
    image_count = swap_chain_support.capabilities_.maxImageCount;
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
  uint32_t queue_family_indices[] = {indices.graphics_family_.value(),
                                     indices.present_family_.value()};
  if (indices.graphics_family_!=indices.present_family_) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }
  create_info.preTransform = swap_chain_support.capabilities_.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;
  if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }
  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, swap_chain_images_.data());
}

VkSurfaceFormatKHR
VulkanApplication::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats) {
  for (const auto &available_format : available_formats) {
    if (available_format.format==VK_FORMAT_B8G8R8A8_UNORM &&
        available_format.colorSpace==VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return available_format;
    }
  }
  return available_formats[0];
}

VkPresentModeKHR
VulkanApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes) {
  for (const auto &available_present_mode : available_present_modes) {
    if (available_present_mode==VK_PRESENT_MODE_MAILBOX_KHR) {
      return available_present_mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width!=UINT32_MAX) {
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

void VulkanApplication::CreateImageViews() {
  swap_chain_image_views_.resize(swap_chain_images_.size());
  for (size_t i = 0; i < swap_chain_images_.size(); i++) {
    swap_chain_image_views_[i] = context_->CreateImageView(swap_chain_images_[i], swap_chain_image_format_);
  }
}

void VulkanApplication::CreateRenderPass() {
  VkAttachmentDescription color_attachment = {};
  color_attachment.format = swap_chain_image_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription sub_pass = {};
  sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sub_pass.colorAttachmentCount = 1;
  sub_pass.pColorAttachments = &color_attachment_ref;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &sub_pass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
  context_->SetVkRenderPass(&render_pass_);
}

void VulkanApplication::CreateFrameBuffers() {
  swap_chain_frame_buffers_.resize(swap_chain_image_views_.size());
  for (size_t i = 0; i < swap_chain_image_views_.size(); i++) {
    VkImageView attachments[] = {swap_chain_image_views_[i]};
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;
    if (vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &swap_chain_frame_buffers_[i])!=VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void VulkanApplication::CreateCommandBuffers() {
  graphics_command_buffers_.resize(swap_chain_frame_buffers_.size());
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = graphics_command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = (uint32_t) graphics_command_buffers_.size();
  if (vkAllocateCommandBuffers(device_, &alloc_info, graphics_command_buffers_.data())!=VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void VulkanApplication::CreateSyncObjects() {
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
    if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphores_[i])!=VK_SUCCESS ||
        vkCreateSemaphore(device_, &semaphore_info, nullptr, &render_finished_semaphores_[i])!=VK_SUCCESS ||
        vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[i])!=VK_SUCCESS) {
      throw std::runtime_error("failed to create sync objects for a frame!");
    }
  }
}

void VulkanApplication::InitImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForVulkan(window_, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vulkan_instance_;
  init_info.PhysicalDevice = physical_device_;
  init_info.Device = device_;
  init_info.Queue = graphics_queue_;
  init_info.Allocator = nullptr;
  init_info.MinImageCount = 2;
  init_info.DescriptorPool = descriptor_pool_;
  init_info.ImageCount = max_frames_in_flight_;
  init_info.CheckVkResultFn = CheckVkResult;
  ImGui_ImplVulkan_Init(&init_info, render_pass_);

  VkCommandBuffer buffer = context_->BeginSingleTimeCommands(graphics_command_pool_);
  ImGui_ImplVulkan_CreateFontsTexture(buffer);
  context_->EndSingleTimeCommands(graphics_queue_, graphics_command_pool_, buffer);
  ImGui_ImplVulkan_DestroyFontUploadObjects();

  ImGui::StyleColorsDark();
}

bool VulkanApplication::PrepareFrame() {
  if (framebuffer_resized_) {
    RecreateSwapChain();
    framebuffer_resized_ = false;
  }
  vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

  VkResult result = vkAcquireNextImageKHR(device_, swap_chain_, UINT64_MAX, image_available_semaphores_[current_frame_],
                                          VK_NULL_HANDLE, &current_image_);

  if (result==VK_ERROR_OUT_OF_DATE_KHR) {
    framebuffer_resized_ = true;
    return false;
  } else if (result!=VK_SUCCESS && result!=VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  if (images_in_flight_[current_image_]!=VK_NULL_HANDLE) {
    vkWaitForFences(device_, 1, &images_in_flight_[current_image_], VK_TRUE, UINT64_MAX);
  }
  images_in_flight_[current_image_] = in_flight_fences_[current_frame_];

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(graphics_command_buffers_[current_frame_], &begin_info);

  VkRenderPassBeginInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass_;
  render_pass_info.framebuffer = swap_chain_frame_buffers_[current_image_];
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = swap_chain_extent_;

  render_pass_info.clearValueCount = 1;
  float *color = renderer_->GetColor();
  VkClearValue clear_color = {color[0], color[1], color[2], color[3]};
  render_pass_info.pClearValues = &clear_color;
  vkCmdBeginRenderPass(graphics_command_buffers_[current_frame_], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
  context_->SetCurrentCommandBuffer(&graphics_command_buffers_[current_frame_]);
  return true;
}

void VulkanApplication::CreateImGuiFrame() {
  ImGui_ImplVulkan_NewFrame();
}

void VulkanApplication::RenderImGui() {
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), graphics_command_buffers_[current_frame_]);
}

void VulkanApplication::DrawFrame() {
  vkCmdEndRenderPass(graphics_command_buffers_[current_frame_]);
  vkEndCommandBuffer(graphics_command_buffers_[current_frame_]);
  context_->SetCurrentCommandBuffer(nullptr);

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
  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_])!=VK_SUCCESS) {
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

  if (result==VK_ERROR_OUT_OF_DATE_KHR || result==VK_SUBOPTIMAL_KHR) {
    framebuffer_resized_ = true;
  } else if (result!=VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  current_frame_ = (current_frame_ + 1)%max_frames_in_flight_;
  context_->SetCurrentImageIndex(current_frame_);
}

void VulkanApplication::PrepareForShutdown() {
  vkDeviceWaitIdle(device_);
}

void VulkanApplication::DestroyImGui() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void VulkanApplication::CleanupSwapChain() {
  for (auto framebuffer : swap_chain_frame_buffers_) {
    vkDestroyFramebuffer(device_, framebuffer, nullptr);
  }
  vkFreeCommandBuffers(device_,
                       graphics_command_pool_,
                       static_cast<uint32_t>(graphics_command_buffers_.size()),
                       graphics_command_buffers_.data());
  vkDestroyRenderPass(device_, render_pass_, nullptr);
  context_->SetVkRenderPass(nullptr);
  for (auto image_view : swap_chain_image_views_) {
    vkDestroyImageView(device_, image_view, nullptr);
  }
  vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
}

void VulkanApplication::DestroyContext() {
  DeleteTestMenu();
  delete context_;
  CleanupSwapChain();
  for (size_t i = 0; i < max_frames_in_flight_; i++) {
    vkDestroySemaphore(device_, render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(device_, image_available_semaphores_[i], nullptr);
    vkDestroyFence(device_, in_flight_fences_[i], nullptr);
  }
  vkDestroyCommandPool(device_, graphics_command_pool_, nullptr);
  vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
  vkDestroyDevice(device_, nullptr);
  vkDestroySurfaceKHR(vulkan_instance_, surface_, nullptr);
  if (enable_validation_layers_) {
    DestroyDebugUtilsMessengerExt(vulkan_instance_, debug_messenger_, nullptr);
  }
  vkDestroyInstance(vulkan_instance_, nullptr);
}
}
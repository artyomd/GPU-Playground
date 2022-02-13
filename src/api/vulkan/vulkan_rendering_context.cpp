//
// Created by artyomd on 12/31/19.
//

#include "src/api/vulkan/vulkan_rendering_context.hpp"

#include <stdexcept>

#include "src/api/vulkan/vulkan_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_pipeline.hpp"
#include "src/api/vulkan/vulkan_shader.hpp"
#include "src/api/vulkan/vulkan_texture_2d.hpp"

api::vulkan::VulkanRenderingContext::VulkanRenderingContext(
    VkPhysicalDevice physical_device, VkDevice device, VkQueue graphics_queue,
    VkCommandPool graphics_pool, VkFormat color_attachment_format,
    uint32_t image_count)
    : RenderingContext(),
      color_attachment_format_(color_attachment_format),
      physical_device_(physical_device),
      device_(device),
      graphics_queue_(graphics_queue),
      graphics_pool_(graphics_pool),
      image_count_(image_count) {
  depth_attachment_format_ = FindSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

  /// descriptor pool
  std::vector<VkDescriptorPoolSize> pool_sizes = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 50},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 50}};
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 50 * static_cast<uint32_t>(pool_sizes.size());
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes = pool_sizes.data();
  if (vkCreateDescriptorPool(device_, &pool_info, nullptr, &descriptor_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

  /// render pass
  VkAttachmentDescription depth_attachment = {};
  depth_attachment.format = depth_attachment_format_;
  depth_attachment.samples = VK_SAMPLE_COUNT_4_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depth_attachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription color_attachment = {};
  color_attachment.format = color_attachment_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_4_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription color_attachment_resolve = {};
  color_attachment_resolve.format = color_attachment_format_;
  color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_resolve_ref = {};
  color_attachment_resolve_ref.attachment = 2;
  color_attachment_resolve_ref.layout =
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription sub_pass = {};
  sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sub_pass.colorAttachmentCount = 1;
  sub_pass.pColorAttachments = &color_attachment_ref;
  sub_pass.pDepthStencilAttachment = &depth_attachment_ref;
  sub_pass.pResolveAttachments = &color_attachment_resolve_ref;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      color_attachment, depth_attachment, color_attachment_resolve};

  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &sub_pass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

VkSampleCountFlagBits
api::vulkan::VulkanRenderingContext::GetMaxUsableSampleCount() {
  VkPhysicalDeviceProperties physical_device_properties;
  vkGetPhysicalDeviceProperties(physical_device_, &physical_device_properties);
  VkSampleCountFlags counts =
      std::min(physical_device_properties.limits.framebufferColorSampleCounts,
               physical_device_properties.limits.framebufferDepthSampleCounts);
  if (counts & VK_SAMPLE_COUNT_64_BIT) {
    return VK_SAMPLE_COUNT_64_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_32_BIT) {
    return VK_SAMPLE_COUNT_32_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_16_BIT) {
    return VK_SAMPLE_COUNT_16_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_8_BIT) {
    return VK_SAMPLE_COUNT_8_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_4_BIT) {
    return VK_SAMPLE_COUNT_4_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_2_BIT) {
    return VK_SAMPLE_COUNT_2_BIT;
  }
  return VK_SAMPLE_COUNT_1_BIT;
}

VkDevice api::vulkan::VulkanRenderingContext::GetDevice() const {
  return device_;
}

std::shared_ptr<api::Buffer> api::vulkan::VulkanRenderingContext::CreateBuffer(
    size_t size_in_bytes, api::BufferUsage usage, api::MemoryType memory_type) {
  return std::make_shared<VulkanBuffer>(shared_from_this(), size_in_bytes,
                                        GetVkBufferUsage(usage),
                                        GetVkMemoryType(memory_type));
}

std::shared_ptr<api::RenderingPipeline>
api::vulkan::VulkanRenderingContext::CreateGraphicsPipeline(
    std::shared_ptr<Shader> vertex_shader,
    std::shared_ptr<Shader> fragment_shader, const api::VertexBufferLayout &vbl,
    api::RenderingPipelineConfig config) {
  return std::make_shared<VulkanRenderingPipeline>(
      shared_from_this(), vertex_shader, fragment_shader, vbl, config);
}

std::shared_ptr<api::Shader> api::vulkan::VulkanRenderingContext::CreateShader(
    std::string sipr_v_shader_location, std::string entry_point_name,
    api::ShaderType type) {
  return std::make_shared<VulkanShader>(
      shared_from_this(), sipr_v_shader_location, entry_point_name, type);
}

std::shared_ptr<api::Texture2D>
api::vulkan::VulkanRenderingContext::CreateTexture2D(uint32_t width,
                                                     uint32_t height,
                                                     PixelFormat pixel_format) {
  return std::make_shared<VulkanTexture2D>(shared_from_this(), width, height,
                                           pixel_format);
}

void api::vulkan::VulkanRenderingContext::WaitForGpuIdle() const {
  vkDeviceWaitIdle(device_);
}

uint32_t api::vulkan::VulkanRenderingContext::GetCurrentImageIndex() const {
  return current_image_index_;
}

void api::vulkan::VulkanRenderingContext::SetCurrentImageIndex(
    uint32_t current_image_index) {
  current_image_index_ = current_image_index;
}

VkFormat api::vulkan::VulkanRenderingContext::FindSupportedFormat(
    const std::vector<VkFormat> &candidates, VkImageTiling tiling,
    VkFormatFeatureFlags features) const {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);
    if ((tiling == VK_IMAGE_TILING_LINEAR &&
         (props.linearTilingFeatures & features) == features) ||
        (tiling == VK_IMAGE_TILING_OPTIMAL &&
         (props.optimalTilingFeatures & features) == features)) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}
void api::vulkan::VulkanRenderingContext::CreateImage(
    uint32_t width, uint32_t height, VkSampleCountFlagBits num_samples,
    VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
    VkImage *image, VkDeviceMemory *image_memory) const {
  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = num_samples;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device_, &image_info, nullptr, image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(device_, *image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex =
      FindMemoryType(mem_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory((device_), &alloc_info, nullptr, image_memory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(device_, *image, *image_memory, 0);
}

VkRenderPass api::vulkan::VulkanRenderingContext::GetRenderPass() const {
  return render_pass_;
}

void api::vulkan::VulkanRenderingContext::CopyBufferToImage(VkBuffer buffer,
                                                            VkImage image,
                                                            size_t width,
                                                            size_t height) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommands(graphics_pool_);

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height), 1};

  vkCmdCopyBufferToImage(command_buffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  EndSingleTimeCommands(graphics_queue_, graphics_pool_, command_buffer);
}

void api::vulkan::VulkanRenderingContext::TransitionImageLayout(
    VkImage image, VkImageLayout old_layout, VkImageLayout new_layout) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommands(graphics_pool_);

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;
  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
             new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
             new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }
  vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  EndSingleTimeCommands(graphics_queue_, graphics_pool_, command_buffer);
}

void api::vulkan::VulkanRenderingContext::CreateBuffer(
    VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkBuffer *buffer,
    VkDeviceMemory *buffer_memory) {
  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (vkCreateBuffer(device_, &buffer_info, nullptr, buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }
  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(device_, *buffer, &mem_requirements);
  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex =
      FindMemoryType(mem_requirements.memoryTypeBits, properties);
  if (vkAllocateMemory(device_, &alloc_info, nullptr, buffer_memory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }
  vkBindBufferMemory(device_, *buffer, *buffer_memory, 0);
}

VkDescriptorPool api::vulkan::VulkanRenderingContext::GetDescriptorPool() {
  return descriptor_pool_;
}
uint32_t api::vulkan::VulkanRenderingContext::GetImageCount() const {
  return image_count_;
}

void api::vulkan::VulkanRenderingContext::CopyBuffer(VkBuffer src_buffer,
                                                     VkBuffer dst_buffer,
                                                     VkDeviceSize size,
                                                     VkDeviceSize src_offset,
                                                     VkDeviceSize dst_offset) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommands(graphics_pool_);
  VkBufferCopy copy_region = {};
  copy_region.size = size;
  copy_region.srcOffset = src_offset;
  copy_region.dstOffset = dst_offset;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
  EndSingleTimeCommands(graphics_queue_, graphics_pool_, command_buffer);
}

uint32_t api::vulkan::VulkanRenderingContext::FindMemoryType(
    uint32_t type_filter, VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);

  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if (type_filter & (1u << i) &&
        (mem_properties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type!");
}

void api::vulkan::VulkanRenderingContext::CreateImageView(
    VkImage image, VkFormat format, VkImageAspectFlagBits aspect_mask,
    VkImageView *image_view) {
  VkImageViewCreateInfo view_info = {};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.subresourceRange.aspectMask = aspect_mask;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;
  if (vkCreateImageView(device_, &view_info, nullptr, image_view) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }
}

VkCommandBuffer api::vulkan::VulkanRenderingContext::BeginSingleTimeCommands(
    VkCommandPool command_pool) {
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool;
  alloc_info.commandBufferCount = 1;
  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer);
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &begin_info);
  return command_buffer;
}

void api::vulkan::VulkanRenderingContext::EndSingleTimeCommands(
    VkQueue queue, VkCommandPool pool, VkCommandBuffer command_buffer) {
  vkEndCommandBuffer(command_buffer);
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;
  vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);
  vkFreeCommandBuffers(device_, pool, 1, &command_buffer);
}

VkCommandBuffer api::vulkan::VulkanRenderingContext::GetCurrentCommandBuffer()
    const {
  return current_command_buffer_;
}

void api::vulkan::VulkanRenderingContext::SetCurrentCommandBuffer(
    VkCommandBuffer current_command_buffer) {
  current_command_buffer_ = current_command_buffer;
}

api::vulkan::VulkanRenderingContext::~VulkanRenderingContext() {
  vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
  vkDestroyRenderPass(device_, render_pass_, nullptr);
}

VkFormat api::vulkan::VulkanRenderingContext::GetDepthAttachmentFormat() const {
  return depth_attachment_format_;
}

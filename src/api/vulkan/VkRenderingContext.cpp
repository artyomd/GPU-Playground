//
// Created by artyomd on 12/31/19.
//

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vendor/glm/ext.hpp>
#include "VkRenderingContext.h"
#include "VkVertexBuffer.h"
#include "VkIndexBuffer.h"
#include "VkVertexBinding.h"
#include "VkShader.h"
#include "VkRenderingPipeline.h"
#include "VkUniformBuffer.h"

namespace api {
VkRenderingContext::VkRenderingContext(
    VkPhysicalDevice *physical_device,
    VkDevice *device,
    VkCommandPool *graphics_pool,
    VkQueue *graphics_queue,
    VkDescriptorPool *descriptor_pool,
    int image_count) :
    RenderingContext(),
    physical_device_(physical_device),
    device_(device),
    graphics_pool_(graphics_pool),
    graphics_queue_(graphics_queue),
    descriptor_pool_(descriptor_pool),
    image_count_(image_count) {
}

IndexBuffer *VkRenderingContext::CreateIndexBuffer(const void *data, unsigned int size, DataType type) {
  return new VkIndexBuffer(this, data, size, type);
}

void VkRenderingContext::FreeIndexBuffer(IndexBuffer *buffer) {
  delete buffer;
}
VertexBuffer *VkRenderingContext::CreateVertexBuffer(const void *data, unsigned int size) {
  return new VkVertexBuffer(this, data, size);
}

void VkRenderingContext::FreeVertexBuffer(VertexBuffer *buffer) {
  delete buffer;
}

VkDevice *VkRenderingContext::GetDevice() const {
  return device_;
}

VkRenderPass *VkRenderingContext::GetVkRenderPass() const {
  return vk_render_pass_;
}
VkExtent2D *VkRenderingContext::GetSwapChainExtent() const {
  return swap_chain_extent_;
}

void VkRenderingContext::CreateBuffer(VkDeviceSize size,
                                      VkBufferUsageFlags usage,
                                      VkMemoryPropertyFlags properties,
                                      VkBuffer &buffer,
                                      VkDeviceMemory &buffer_memory) {
  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size;
  buffer_info.usage = usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (vkCreateBuffer(*device_, &buffer_info, nullptr, &buffer)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }
  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(*device_, buffer, &mem_requirements);
  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
  if (vkAllocateMemory(*device_, &alloc_info, nullptr, &buffer_memory)!=VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }
  vkBindBufferMemory(*device_, buffer, buffer_memory, 0);
}

VkDescriptorPool *VkRenderingContext::GetDescriptorPool() {
  return descriptor_pool_;
}
int VkRenderingContext::GetImageCount() {
  return image_count_;
}

void VkRenderingContext::CopyBuffer(VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize size) {
  VkCommandBuffer command_buffer = BeginSingleTimeCommands(*graphics_pool_);
  VkBufferCopy copy_region = {};
  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
  EndSingleTimeCommands(*graphics_queue_, *graphics_pool_, command_buffer);
}

uint32_t VkRenderingContext::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(*physical_device_, &mem_properties);

  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if (type_filter & (1u << i)
        && (mem_properties.memoryTypes[i].propertyFlags & properties)==properties) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type!");
}

VkImageView VkRenderingContext::CreateImageView(VkImage &image, VkFormat format) {
  VkImageViewCreateInfo view_info = {};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;
  VkImageView image_view;
  if (vkCreateImageView(*device_, &view_info, nullptr, &image_view)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }
  return image_view;
}

VkCommandBuffer VkRenderingContext::BeginSingleTimeCommands(VkCommandPool &command_pool) {
  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool;
  alloc_info.commandBufferCount = 1;
  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(*device_, &alloc_info, &command_buffer);
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &begin_info);
  return command_buffer;
}

void
VkRenderingContext::EndSingleTimeCommands(VkQueue &queue, VkCommandPool &pool, VkCommandBuffer &command_buffer) {
  vkEndCommandBuffer(command_buffer);
  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;
  vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);
  vkFreeCommandBuffers(*device_, pool, 1, &command_buffer);
}
VertexBinding *VkRenderingContext::CreateVertexBinding(const VertexBuffer *buffer,
                                                       const VertexBufferLayout *vertex_buffer_layout) {
  return new VkVertexBinding(buffer, vertex_buffer_layout);
}
void VkRenderingContext::FreeVertexBiding(VertexBinding *vertex_binding) {
  delete vertex_binding;
}

RenderingPipeline *VkRenderingContext::CreateGraphicsPipeline(const VertexBinding *vertex_binding,
                                                              const IndexBuffer *index_buffer,
                                                              const Shader *vertex_shader,
                                                              const Shader *fragment_shader,
                                                              const UniformBuffer *shader_properties) {
  return new VkRenderingPipeline(this, vertex_binding, index_buffer, vertex_shader, fragment_shader, shader_properties);
}
void VkRenderingContext::FreeGraphicsPipeline(RenderingPipeline *pipeline) {
  delete pipeline;

}
Shader *VkRenderingContext::CreateShader(std::string sipr_v_shader_location,
                                         std::string glsl_location,
                                         std::string entry_point_name,
                                         api::ShaderType type) {
  return new VkShader(this, sipr_v_shader_location, glsl_location, entry_point_name, type);
}
void VkRenderingContext::DeleteShader(Shader *shader) {
  delete shader;
}
VkCommandBuffer *VkRenderingContext::GetCurrentCommandBuffer() const {
  return current_command_buffer_;
}
void VkRenderingContext::SetCurrentCommandBuffer(VkCommandBuffer *current_command_buffer) {
  current_command_buffer_ = current_command_buffer;
}
void VkRenderingContext::SetVkRenderPass(VkRenderPass *vk_render_pass) {
  vk_render_pass_ = vk_render_pass;
}
void VkRenderingContext::SetSwapChainExtent(VkExtent2D *swap_chain_extent) {
  swap_chain_extent_ = swap_chain_extent;
  float new_width = 4.0f;
  float new_height = (swap_chain_extent->height * new_width) / swap_chain_extent->width;
  ortho_projection_ = glm::ortho(-new_width, new_width, new_height, -new_height);
}
UniformBuffer *VkRenderingContext::CreateUniformBuffer(int length,
                                                       int binding_point,
                                                       ShaderType shader_stage) {
  return new VkUniformBuffer(this, length, binding_point, shader_stage);
}
void VkRenderingContext::DeleteUniformBuffer(UniformBuffer *uniform_buffer) {
  delete uniform_buffer;
}
int VkRenderingContext::GetCurrentImageIndex() const {
  return current_image_index_;
}
void VkRenderingContext::SetCurrentImageIndex(int current_image_index) {
  current_image_index_ = current_image_index;
}
}
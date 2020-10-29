//
// Created by artyomd on 12/30/19.
//

#include "src/api/vulkan/vulkan_vertex_buffer.hpp"

#include <cstring>
#include <utility>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanVertexBuffer::VulkanVertexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                    size_t size_in_bytes,
                                                    const VertexBufferLayout &layout)
    : VertexBuffer(size_in_bytes),
      context_(std::move(context)),
      device_(context_->GetDevice()) {

  context_->CreateBuffer(size_in_bytes_,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         &buffer_,
                         &memory_);

  vertex_input_binding_description_.binding = 0;
  vertex_input_binding_description_.stride = layout.GetStride();
  vertex_input_binding_description_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  const auto &elements = layout.GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];

    VkVertexInputAttributeDescription description{};
    description.binding = 0;
    description.location = i;
    description.format = GetVkFormat(element.type, element.count);
    description.offset = offset;
    attribute_descriptions_.push_back(description);

    offset += element.count * GetDataTypeSizeInBytes(element.type);
  }

  vertex_input_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info_.vertexBindingDescriptionCount = 1;
  vertex_input_info_.pVertexBindingDescriptions = &vertex_input_binding_description_;
  vertex_input_info_.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions_.size());
  vertex_input_info_.pVertexAttributeDescriptions = attribute_descriptions_.data();

}

void api::vulkan::VulkanVertexBuffer::Update(void *data) {
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  context_->CreateBuffer(size_in_bytes_,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &staging_buffer,
                         &staging_buffer_memory);

  void *mapped_data;
  vkMapMemory(device_, staging_buffer_memory, 0, size_in_bytes_, 0, &mapped_data);
  memcpy(mapped_data, data, size_in_bytes_);
  vkUnmapMemory(device_, staging_buffer_memory);
  context_->CopyBuffer(staging_buffer, buffer_, size_in_bytes_);
  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

VkPipelineVertexInputStateCreateInfo api::vulkan::VulkanVertexBuffer::GetVertexInputInfo() const {
  return vertex_input_info_;
}

const VkBuffer *api::vulkan::VulkanVertexBuffer::GetBuffer() const {
  return &buffer_;
}

api::vulkan::VulkanVertexBuffer::~VulkanVertexBuffer() {
  vkDestroyBuffer(device_, buffer_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

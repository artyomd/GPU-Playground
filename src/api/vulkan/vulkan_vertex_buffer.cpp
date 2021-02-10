//
// Created by artyomd on 12/30/19.
//

#include "src/api/vulkan/vulkan_vertex_buffer.hpp"

#include <utility>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanVertexBuffer::VulkanVertexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                    size_t size_in_bytes,
                                                    const VertexBufferLayout &layout)
    : Buffer(size_in_bytes),
      VulkanBuffer(context, size_in_bytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      VertexBuffer(size_in_bytes) {

  const auto &elements = layout.GetElements();
  auto stride = static_cast<uint32_t>(layout.GetElementSize());
  size_t offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];

    VkVertexInputBindingDescription vertex_input_binding_description{};
    vertex_input_binding_description.binding = i;
    vertex_input_binding_description.stride = stride;
    vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertex_input_binding_descriptions_.push_back(vertex_input_binding_description);

    VkVertexInputAttributeDescription description{};
    description.binding = i;
    description.location = element.binding_index;
    description.format = GetVkFormat(element.type, element.count);
    description.offset = offset;
    attribute_descriptions_.push_back(description);
    offset += element.count * GetDataTypeSizeInBytes(element.type);
  }

  vertex_input_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info_.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_binding_descriptions_.size());
  vertex_input_info_.pVertexBindingDescriptions = vertex_input_binding_descriptions_.data();
  vertex_input_info_.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions_.size());
  vertex_input_info_.pVertexAttributeDescriptions = attribute_descriptions_.data();
}

VkPipelineVertexInputStateCreateInfo api::vulkan::VulkanVertexBuffer::GetVertexInputInfo() const {
  return vertex_input_info_;
}

void api::vulkan::VulkanVertexBuffer::BindBuffer(const VkCommandBuffer &command_buffer) {
  VkDeviceSize offsets[] = {0};
  for (int i = 0; i < vertex_input_binding_descriptions_.size(); i++) {
    vkCmdBindVertexBuffers(command_buffer, i, 1, &buffer_, offsets);
  }
}

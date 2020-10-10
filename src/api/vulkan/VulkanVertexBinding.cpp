//
// Created by artyomd on 1/1/20.
//

#include "src/api/vulkan/VulkanVertexBinding.h"

#include <utility>

#include "src/api/vulkan/VulkanUtils.h"

api::VulkanVertexBinding::VulkanVertexBinding(std::shared_ptr<api::VertexBuffer> vb,
                                              std::shared_ptr<api::VertexBufferLayout> buffer_description)
    : VertexBinding(std::move(vb), std::move(buffer_description)) {
  vertex_input_binding_description_.binding = 0;
  vertex_input_binding_description_.stride = buffer_description_->GetStride();
  vertex_input_binding_description_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  const auto &elements = buffer_description_->GetElements();
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
}
const VkVertexInputBindingDescription *api::VulkanVertexBinding::GetVertexInputBindingDescription() const {
  return &vertex_input_binding_description_;
}
const std::vector<VkVertexInputAttributeDescription> *api::VulkanVertexBinding::GetAttributeDescriptions() const {
  return &attribute_descriptions_;
}
void api::VulkanVertexBinding::Bind() const {
  buffer_->Bind();
}
void api::VulkanVertexBinding::Unbind() const {
  buffer_->Unbind();
}

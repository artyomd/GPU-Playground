//
// Created by artyomd on 1/1/20.
//

#include "src/api/vulkan/VkVertexBinding.h"

api::VkVertexBinding::VkVertexBinding(const api::VertexBuffer *vb, const api::VertexBufferLayout *layout)
    : VertexBinding(vb, layout) {
  vertex_input_binding_description_ = new VkVertexInputBindingDescription();
  vertex_input_binding_description_->binding = 0;
  vertex_input_binding_description_->stride = layout->GetStride();
  vertex_input_binding_description_->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  attribute_descriptions_ = new std::vector<VkVertexInputAttributeDescription>();

  const auto &elements = layout->GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];

    VkVertexInputAttributeDescription description{};
    description.binding = 0;
    description.location = i;
    description.format = GetVkFormat(element.type_, element.count_);
    description.offset = offset;
    attribute_descriptions_->push_back(description);

    offset += element.count_ * GetDataTypeSizeInBytes(element.type_);
  }
}
const VkVertexInputBindingDescription *api::VkVertexBinding::GetVertexInputBindingDescription() const {
  return vertex_input_binding_description_;
}
const std::vector<VkVertexInputAttributeDescription> *api::VkVertexBinding::GetAttributeDescriptions() const {
  return attribute_descriptions_;
}
void api::VkVertexBinding::Bind() const {
  buffer_->Bind();
}
void api::VkVertexBinding::Unbind() const {
  buffer_->Unbind();
}
api::VkVertexBinding::~VkVertexBinding() {
  delete vertex_input_binding_description_;
  delete attribute_descriptions_;
}

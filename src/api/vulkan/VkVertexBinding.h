//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <vulkan/vulkan.h>
#include "VertexBinding.h"

namespace api {
class VkVertexBinding : public VertexBinding {
  VkVertexInputBindingDescription* vertex_input_binding_description_;
  std::vector<VkVertexInputAttributeDescription>* attribute_descriptions_;

 public:
  VkVertexBinding(const VertexBuffer *vb, const VertexBufferLayout *layout);

  const VkVertexInputBindingDescription* GetVertexInputBindingDescription() const;

  const std::vector<VkVertexInputAttributeDescription>* GetAttributeDescriptions() const;

  void Bind() const override;
  void Unbind() const override;
  ~VkVertexBinding() override;
};
}

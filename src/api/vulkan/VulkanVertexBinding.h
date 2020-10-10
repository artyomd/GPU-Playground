//
// Created by artyomd on 1/1/20.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/VertexBinding.h"

namespace api {
class VulkanVertexBinding : public VertexBinding {
  VkVertexInputBindingDescription vertex_input_binding_description_{};
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions_{};

 public:
  VulkanVertexBinding(std::shared_ptr<VertexBuffer> vb, std::shared_ptr<VertexBufferLayout> buffer_description);

  [[nodiscard]] const VkVertexInputBindingDescription *GetVertexInputBindingDescription() const;

  [[nodiscard]] const std::vector<VkVertexInputAttributeDescription> *GetAttributeDescriptions() const;

  void Bind() const override;
  void Unbind() const override;
};
}

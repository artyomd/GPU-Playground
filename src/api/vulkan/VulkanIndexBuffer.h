//
// Created by artyomd on 2/25/20.
//

#include <vulkan/vulkan.h>

#include "src/api/IndexBuffer.h"
#include "src/api/vulkan/VulkanRenderingContext.h"

namespace api {
class VulkanIndexBuffer : public IndexBuffer {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;

  VkBuffer buffer_ = nullptr;
  VkDeviceMemory memory_ = nullptr;
  VkIndexType index_type_;

 public:
  VulkanIndexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                    const void *indices_data,
                    unsigned int item_count,
                    enum DataType type
  );

  void Bind() const override;

  void Unbind() const override;

  ~VulkanIndexBuffer() override;
};
}

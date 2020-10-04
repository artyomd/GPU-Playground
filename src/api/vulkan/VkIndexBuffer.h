//
// Created by artyomd on 2/25/20.
//

#include <vulkan/vulkan.h>

#include "src/api/IndexBuffer.h"
#include "src/api/vulkan/VkRenderingContext.h"

namespace api {
class VkIndexBuffer : public IndexBuffer {
 private:
  VkRenderingContext *context_;
  VkDevice *device_;
  VkBuffer buffer_;
  VkDeviceMemory memory_;
  VkIndexType index_type_;

 public:
  VkIndexBuffer(VkRenderingContext *context,
                const void *indices_data,
                unsigned int item_count,
                enum DataType type
  );

  void Bind() const override;

  void Unbind() const override;

  ~VkIndexBuffer() override;
};
}

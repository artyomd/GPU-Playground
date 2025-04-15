#pragma once

#include "image_view.hpp"
#include "rendering_context.hpp"
#include "shader.hpp"

namespace vulkan {

class ComputePipeline final {
 public:
  static std::shared_ptr<ComputePipeline> Create(const std::shared_ptr<RenderingContext> &context,
                                                 const std::shared_ptr<Shader> &compute_shader,
                                                 size_t descriptor_set_count);

  ComputePipeline() = delete;
  ComputePipeline(const ComputePipeline &) = delete;
  ComputePipeline(ComputePipeline &&) = delete;

  ComputePipeline &operator=(const ComputePipeline &) = delete;
  ComputePipeline &operator=(ComputePipeline &&) = delete;

  void SetBuffer(uint32_t binding_point, uint32_t descriptor_set_index, const std::shared_ptr<Buffer> &buffer);

  [[nodiscard]] size_t GetDescriptorSetCount() const;

  void Dispatch(const VkCommandBuffer &command_buffer, const uint32_t &group_count_x, const uint32_t &group_count_y,
                const uint32_t &group_count_z, const size_t &descriptor_index) const;

  ~ComputePipeline();

 private:
  ComputePipeline(const std::shared_ptr<RenderingContext> &context, const std::shared_ptr<Shader> &compute_shader,
                  size_t descriptor_set_count);

  std::shared_ptr<RenderingContext> context_ = nullptr;

  std::shared_ptr<Shader> compute_shader_ = nullptr;

  VkPipeline pipeline_ = VK_NULL_HANDLE;
  VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;

  struct DescriptorSetInfo {
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    std::map<uint32_t, std::shared_ptr<Buffer>> uniform_buffers;
    std::map<uint32_t, std::shared_ptr<Buffer>> storage_buffers;
  };
  std::vector<DescriptorSetInfo> descriptor_sets_;
  VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;
};
}  // namespace vulkan

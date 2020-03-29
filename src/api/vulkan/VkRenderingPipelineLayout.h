//
// Created by artyomd on 3/29/20.
//

#pragma once

#include <api/RenderingPipelineLayout.h>
#include "VkRenderingContext.h"
namespace api {
class VkRenderingPipelineLayout : public RenderingPipelineLayout {
 private:
  VkRenderingContext *context_;
  VkDescriptorPool *descriptor_pool_;
  VkDevice *device_;
  int image_count_;
  std::vector<VkDescriptorSet> descriptor_sets_{};
  VkDescriptorSetLayout descriptor_set_layout_{};
  VkPipelineLayout *pipeline_layout_ = new VkPipelineLayout();
 public:
  VkRenderingPipelineLayout(VkRenderingContext *context, const std::vector<Uniform *> &bindings);
  [[nodiscard]] VkPipelineLayout *GetPipelineLayout() const;
  void Bind() const override;
  ~VkRenderingPipelineLayout() override;
};
}
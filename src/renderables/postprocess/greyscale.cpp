#include <imgui.h>

#include "greyscale.hpp"
#include "vulkan/utils.hpp"

renderable::Blur::Blur(const std::shared_ptr<vulkan::RenderingContext>& context, const size_t descriptor_set_count)
    : context_(context), descriptor_set_count_(descriptor_set_count) {
  const std::vector<uint32_t> compute_shader = {
#include "greyscale.spv"

  };
  const auto c_shader = vulkan::Shader::Create(context_, compute_shader, "main");
  pipeline_ = vulkan::ComputePipeline::Create(context_, c_shader, descriptor_set_count_);
  ubos_ = std::vector(descriptor_set_count_,
                      vulkan::Buffer::Create(context_, sizeof(Params), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true));
  for (size_t i = 0; i < descriptor_set_count_; i++) {
    pipeline_->SetBuffer(2, i, ubos_[i]);
  }
}

std::shared_ptr<renderable::Blur> renderable::Blur::Create(const std::shared_ptr<vulkan::RenderingContext>& context,
                                                           const size_t descriptor_set_count) {
  return std::shared_ptr<Blur>(new Blur(context, descriptor_set_count));
}
void renderable::Blur::RenderParams() { ImGui::SliderFloat("fade", &current_.fade, 0, 1); }
uint64_t renderable::Blur::ParamsHash() const { return this->current_.hash(); }

uint64_t renderable::Blur::Process(const VkCommandBuffer& command_buffer,
                                   const std::shared_ptr<vulkan::ImageView>& source,
                                   const std::shared_ptr<vulkan::ImageView>& dst, const size_t descriptor_index) const {
  const auto& ubo = this->ubos_[descriptor_index];
  {
    memcpy(ubo->Map(), &this->current_, sizeof(this->current_));
    ubo->Unmap();
  }
  dst->GetImage()->TransferTo(command_buffer, VK_IMAGE_LAYOUT_GENERAL);
  source->GetImage()->TransferTo(command_buffer, VK_IMAGE_LAYOUT_GENERAL);
  pipeline_->SetImage(0, descriptor_index, source);
  pipeline_->SetImage(1, descriptor_index, dst);
  pipeline_->Dispatch(command_buffer, source->GetWidth() / 16, source->GetHeight() / 16, 1, descriptor_index);
  return this->current_.hash();
}
size_t renderable::Blur::GetDescriptorCount() const { return descriptor_set_count_; }

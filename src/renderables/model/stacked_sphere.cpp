#include "stacked_sphere.hpp"

std::shared_ptr<renderable::StackedSphere> renderable::StackedSphere::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent) {
  return std::shared_ptr<StackedSphere>(new StackedSphere(context, parent));
}

renderable::StackedSphere::StackedSphere(const std::shared_ptr<vulkan::RenderingContext> &context,
                                         const std::shared_ptr<Menu> &parent)
    : Model(context, parent) {
  sphere_ = std::make_shared<geometry::StackedSphere>(context, 1.0F, 32, 64);

  const std::vector<uint32_t> kVertexShader = {
#include "default_mvp_color_vertex_shader.spv"

  };
  v_shader_ = vulkan::Shader::Create(context, kVertexShader, "main");

  const std::vector<uint32_t> kFragmentShader = {
#include "default_color_fragment_shader.spv"

  };
  f_shader_ = vulkan::Shader::Create(context, kFragmentShader, "main");
}

std::shared_ptr<vulkan::RenderingPipeline> renderable::StackedSphere::CreatePipeline(
    const std::shared_ptr<vulkan::RenderingContext> &rendering_context,
    const std::shared_ptr<vulkan::RenderPass> &render_pass, const size_t &descriptor_set_count,
    const VkSampleCountFlagBits &sample_count) {
  auto pipeline =
      vulkan::RenderingPipeline::Create(rendering_context, render_pass, v_shader_, f_shader_, sphere_->GetVbl(),
                                        {
                                            .draw_mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                                            .cull_mode = VK_CULL_MODE_BACK_BIT,
                                            .front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                            .enable_depth_test = VK_FALSE,
                                            .depth_function = VK_COMPARE_OP_LESS,
                                            .sample_count = sample_count,
                                        },
                                        descriptor_set_count);
  pipeline->SetVertexBuffer(sphere_->GetVertexBuffer());
  pipeline->SetIndexBuffer(sphere_->GetIndexBuffer(), sphere_->GetIndexBufferDataType());
  return pipeline;
}
size_t renderable::StackedSphere::NumOfIndicesToDraw() { return sphere_->GetIndexCount(); }
renderable::StackedSphere::~StackedSphere() { WaitForCommandBuffersToFinish(); }

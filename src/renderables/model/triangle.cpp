#include "triangle.hpp"

std::shared_ptr<renderable::Triangle> renderable::Triangle::Create(
    const std::shared_ptr<vulkan::RenderingContext>& context, const std::shared_ptr<Menu>& parent) {
  return std::shared_ptr<Triangle>(new Triangle(context, parent));
}

renderable::Triangle::Triangle(const std::shared_ptr<vulkan::RenderingContext>& context,
                               const std::shared_ptr<Menu>& parent)
    : Model(context, parent) {
  geometry::PointWithColor point_0 = {-0.5F, -0.5F, 0.0F, 255, 0, 0, 255};
  geometry::PointWithColor point_1 = {0.5F, -0.5F, 0.0F, 0, 255, 0, 255};
  geometry::PointWithColor point_2 = {0.0F, 0.5F, 0.0F, 0, 0, 255, 255};

  triangle_ = std::make_shared<geometry::Triangle>(context, point_0, point_1, point_2);

  const std::vector<uint32_t> kVertexShader = {
#include "default_mvp_color_vertex_shader.spv"

  };
  v_shader_ = vulkan::Shader::Create(context, kVertexShader, "main");

  const std::vector<uint32_t> kFragmentShader = {
#include "default_color_fragment_shader.spv"

  };
  f_shader_ = vulkan::Shader::Create(context, kFragmentShader, "main");
}

std::shared_ptr<vulkan::RenderingPipeline> renderable::Triangle::CreatePipeline(
    const std::shared_ptr<vulkan::RenderingContext>& rendering_context,
    const std::shared_ptr<vulkan::RenderPass>& render_pass, const size_t& descriptor_set_count,
    const VkSampleCountFlagBits& sample_count) {
  auto pipeline =
      vulkan::RenderingPipeline::Create(rendering_context, render_pass, v_shader_, f_shader_, triangle_->GetVbl(),
                                        {
                                            .draw_mode = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                            .cull_mode = VK_CULL_MODE_NONE,
                                            .front_face = VK_FRONT_FACE_CLOCKWISE,
                                            .enable_depth_test = VK_FALSE,
                                            .depth_function = VK_COMPARE_OP_LESS,
                                            .sample_count = sample_count,
                                        },
                                        descriptor_set_count);
  pipeline->SetVertexBuffer(triangle_->GetVertexBuffer());
  pipeline->SetIndexBuffer(triangle_->GetIndexBuffer(), triangle_->GetIndexBufferDataType());
  return pipeline;
}

size_t renderable::Triangle::NumOfIndicesToDraw() { return triangle_->GetIndexCount(); }

renderable::Triangle::~Triangle() { WaitForCommandBuffersToFinish(); }

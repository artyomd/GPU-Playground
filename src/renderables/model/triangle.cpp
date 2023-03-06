#include "triangle.hpp"

std::shared_ptr<renderable::Triangle> renderable::Triangle::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                                   std::shared_ptr<Menu> parent) {
  auto triangle = new Triangle(context, parent);
  return std::shared_ptr<Triangle>(triangle);
}

renderable::Triangle::Triangle(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent)
    : Model(context, parent) {

  geometry::Point point_0 = {-0.5F, -0.5F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F};
  geometry::Point point_1 = {0.5F, -0.5F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F};
  geometry::Point point_2 = {0.0F, 0.5F, 0.0F, 0.0F, 0.0F, 1.0F, 1.0F};

  triangle_ = std::make_shared<geometry::Triangle>(context, point_0, point_1, point_2);
  v_shader_ = vulkan::Shader::Create(context,
                                     SHADER_DIR + std::string("default_mvp_color_vertex_shader.glsl"),
                                     "main",
                                     VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
  f_shader_ = vulkan::Shader::Create(context,
                                     SHADER_DIR + std::string("default_color_fragment_shader.glsl"),
                                     "main",
                                     VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
}

std::shared_ptr<vulkan::RenderingPipeline> renderable::Triangle::CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                                                std::shared_ptr<vulkan::RenderPass> render_pass,
                                                                                size_t descriptor_set_count,
                                                                                VkSampleCountFlagBits sample_count) {
  auto pipeline = vulkan::RenderingPipeline::Create(rendering_context,
                                                    render_pass,
                                                    v_shader_,
                                                    f_shader_,
                                                    triangle_->GetVbl(),
                                                    {
                                                        .draw_mode = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                        .cull_mode = VK_CULL_MODE_NONE,
                                                        .front_face = VkFrontFace::VK_FRONT_FACE_CLOCKWISE,
                                                        .enable_depth_test = VK_FALSE,
                                                        .depth_function = VkCompareOp::VK_COMPARE_OP_LESS,
                                                        .sample_count = sample_count,
                                                    },
                                                    descriptor_set_count);
  pipeline->SetVertexBuffer(triangle_->GetVertexBuffer());
  pipeline->SetIndexBuffer(triangle_->GetIndexBuffer(),
                           triangle_->GetIndexBufferDataType());
  return pipeline;
}
size_t renderable::Triangle::NumOfIndicesToDraw() {
  return triangle_->GetIndexCount();
}
renderable::Triangle::~Triangle() {
  WaitForCommandBuffersToFinish();
}

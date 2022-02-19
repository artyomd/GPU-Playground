#pragma once

#include <GL/glew.h>
#include <map>

#include "src/api/rendering_pipeline.hpp"

#include "src/api/opengl/opengl_rendering_context.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_buffer.hpp"
#include "src/api/opengl/opengl_texture_2d.hpp"

namespace api::opengl {
class OpenGlRenderingPipeline : public RenderingPipeline {
 private:
  GLuint program_id_ = 0;

  std::shared_ptr<OpenGlBuffer> vertex_buffer_ = nullptr;
  std::shared_ptr<OpenGlBuffer> index_buffer_ = nullptr;

  GLuint vertex_array_ = 0;
  api::VertexBufferLayout vbl_;

  GLenum index_type_ = GL_UNSIGNED_SHORT;
  GLenum draw_mode_ = 0;
  api::RenderingPipelineConfig config_;

  GLsizei viewport_width_ = 0;
  GLsizei viewport_height_ = 0;

  std::map<GLuint, std::shared_ptr<opengl::OpenGlBuffer>> ubos_{};
  std::map<GLuint, std::shared_ptr<OpenglTexture2D>> textures_{};

 public:
  OpenGlRenderingPipeline(std::shared_ptr<Shader> vertex_shader,
                          std::shared_ptr<Shader> fragment_shader,
                          VertexBufferLayout vbl,
                          RenderingPipelineConfig config);
  void SetIndexBuffer(std::shared_ptr<Buffer> buffer, DataType element_type) override;
  void SetVertexBuffer(std::shared_ptr<Buffer> buffer) override;
  void UpdateUniformBuffer(unsigned int binding_point, void *data) override;
  void SetTexture(unsigned int binding_point, std::shared_ptr<api::Texture2D> texture) override;
  void SetViewPort(uint32_t width, uint32_t height) override;
  void Draw(size_t index_count, size_t offset) override;
  ~OpenGlRenderingPipeline() override;
};
}

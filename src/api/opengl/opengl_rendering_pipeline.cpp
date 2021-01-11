//
// Created by artyomd on 1/1/20.
//

#include "src/api/opengl/opengl_rendering_pipeline.hpp"

#include <iostream>

#include "src/api/opengl/opengl_utils.hpp"

static inline void GetProgramInfoLog(GLuint id) {
  int length;
  GL_CALL(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
  if (length != 0) {
    char *message = static_cast<char *>(malloc(static_cast<unsigned long>(length) * sizeof(char)));
    GL_CALL(glGetProgramInfoLog(id, length, &length, message));
    std::cout << "Program creation info message:" << message;
    free(message);
  }
}

api::opengl::OpenGlRenderingPipeline::OpenGlRenderingPipeline(const std::shared_ptr<VertexBuffer> &vertex_buffer,
                                                              const std::shared_ptr<IndexBuffer> &index_buffer,
                                                              const std::shared_ptr<Shader> &vertex_shader,
                                                              const std::shared_ptr<Shader> &fragment_shader,
                                                              api::RenderingPipelineConfig config) :
    RenderingPipeline(),
    config_(config) {
  vertex_buffer_ = std::dynamic_pointer_cast<OpenGlVertexBuffer>(vertex_buffer);
  index_buffer_ = std::dynamic_pointer_cast<OpenGlIndexBuffer>(index_buffer);
  vertex_shader_ = std::dynamic_pointer_cast<OpenGlShader>(vertex_shader);
  fragment_shader_ = std::dynamic_pointer_cast<OpenGlShader>(fragment_shader);

  GL_CALL(program_id_ = glCreateProgram());
  AssertThat(program_id_, snowhouse::Is().Not().EqualTo(0));

  GL_CALL(glAttachShader(program_id_, vertex_shader_->GetShaderId()));
  GL_CALL(glAttachShader(program_id_, fragment_shader_->GetShaderId()));
  GL_CALL(glLinkProgram(program_id_));

  GLint link_status;
  glGetProgramiv(program_id_, GL_LINK_STATUS, (int *) &link_status);
  if (link_status == GL_FALSE) {
    GetProgramInfoLog(program_id_);
    GL_CALL(glDetachShader(program_id_, vertex_shader_->GetShaderId()));
    GL_CALL(glDetachShader(program_id_, fragment_shader_->GetShaderId()));
    throw std::runtime_error("unable to link Gl program");
  }

  GL_CALL(glValidateProgram(program_id_));
  GetProgramInfoLog(program_id_);
  GL_CALL(glDetachShader(program_id_, vertex_shader_->GetShaderId()));
  GL_CALL(glDetachShader(program_id_, fragment_shader_->GetShaderId()));

  GLint active_uniform_blocks = 0;
  GL_CALL(glGetProgramiv(program_id_, GL_ACTIVE_UNIFORM_BLOCKS, &active_uniform_blocks));

  for (int i = 0; i < active_uniform_blocks; i++) {
    GLint binding_point = -1;
    GLint size_in_bytes = 0;
    GL_CALL(glGetActiveUniformBlockiv(program_id_, i, GL_UNIFORM_BLOCK_BINDING, &binding_point));
    AssertThat(binding_point, snowhouse::Is().Not().EqualTo(-1));
    GL_CALL(glGetActiveUniformBlockiv(program_id_, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size_in_bytes));
    AssertThat(size_in_bytes, snowhouse::Is().Not().EqualTo(0));
    auto buffer = std::make_shared<opengl::OpenGlBuffer>(size_in_bytes);
    ubos_.emplace(std::make_pair(binding_point, buffer));
  }
  draw_mode_ = GetGlDrawMode(config_.draw_mode);
}

void api::opengl::OpenGlRenderingPipeline::Render() {
  GL_CALL(glViewport(0, 0, width_, height_));
  GL_CALL(glScissor(0, 0, width_, height_));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_->GetBufferId()));
  GL_CALL(glBindVertexArray(vertex_buffer_->GetVertexArrayId()));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_->GetBufferId()));
  GL_CALL(glUseProgram(program_id_));
  if (config_.enable_depth_test) {
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthFunc(GetGlCompareOp(config_.depth_function)));
  }

  if (config_.cull_mode != CullMode::NONE) {
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glCullFace(GetGlCullMode(config_.cull_mode)));
    GL_CALL(glFrontFace(GetGlFrontFace(config_.front_face)));
  }
  for (const auto &texture : textures_) {
    texture.second->Bind(texture.first);
  }
  for (const auto &ubo : ubos_) {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, ubo.first, ubo.second->GetBufferId()));
  }
  GL_CALL(glDrawElements(draw_mode_,
                         static_cast<GLsizei>(index_buffer_->GetCount()),
                         index_buffer_->GetIndexType(), nullptr));
  for (const auto &ubo : ubos_) {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, ubo.first, 0));
  }
  for (const auto &texture : textures_) {
    texture.second->Unbind(texture.first);
  }
  GL_CALL(glDisable(GL_DEPTH_TEST));
  GL_CALL(glDisable(GL_CULL_FACE));
  GL_CALL(glUseProgram(0));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CALL(glBindVertexArray(0));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void api::opengl::OpenGlRenderingPipeline::SetViewPort(size_t width, size_t height) {
  this->width_ = static_cast<GLsizei>(width);
  this->height_ = static_cast<GLsizei>(height);
}

void api::opengl::OpenGlRenderingPipeline::UpdateUniformBuffer(unsigned int binding_point, void *data) {
  ubos_[binding_point]->Update(data);
}

void api::opengl::OpenGlRenderingPipeline::SetTexture(unsigned int binding_point,
                                                      std::shared_ptr<api::Texture2D> texture) {
  textures_.emplace(std::make_pair(binding_point, std::dynamic_pointer_cast<opengl::OpenglTexture2D>(texture)));
}

api::opengl::OpenGlRenderingPipeline::~OpenGlRenderingPipeline() {
  GL_CALL(glDeleteProgram(program_id_));
}

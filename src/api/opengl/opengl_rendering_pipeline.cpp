//
// Created by artyomd on 1/1/20.
//

#include "src/api/opengl/opengl_rendering_pipeline.hpp"

#include <iostream>

#include "src/api/opengl/opengl_utils.hpp"

static inline void GetProgramInfoLog(int id) {
  int length;
  GL_CALL(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
  if (length != 0) {
    char *message = (char *) malloc(length * sizeof(char));
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
    RenderingPipeline() {
  vertex_buffer_ = std::dynamic_pointer_cast<OpenGlVertexBuffer>(vertex_buffer);
  index_buffer_ = std::dynamic_pointer_cast<OpenGlIndexBuffer>(index_buffer);
  vertex_shader_ = std::dynamic_pointer_cast<OpenGlShader>(vertex_shader);
  fragment_shader_ = std::dynamic_pointer_cast<OpenGlShader>(fragment_shader);

  GL_CALL(program_id_ = glCreateProgram());
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

  if (config.enable_depth_test) {
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthFunc(GetGlCompareOp(config.depth_function)));
  }

  if (config.cull_mode != CullMode::NONE) {
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glCullFace(GetGlCullMode(config.cull_mode)));
    GL_CALL(glFrontFace(GetGlFrontFace(config.front_face)));
  }

  draw_mode_ = GetGlDrawMode(config.draw_mode);
}

void api::opengl::OpenGlRenderingPipeline::Render() {
  GL_CALL(glViewport(0, 0, width_, height_));
  GL_CALL(glScissor(0, 0, width_, height_));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_->GetBufferId()));
  GL_CALL(glBindVertexArray(vertex_buffer_->GetVertexArrayId()));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_->GetBufferId()));
  GL_CALL(glUseProgram(program_id_));
  for (const auto &uniform : uniforms_) {
    uniform->Bind();
  }
  GL_CALL(glDrawElements(draw_mode_, index_buffer_->GetCount(), index_buffer_->GetIndexType(), nullptr));
  for (const auto &uniform : uniforms_) {
    uniform->Unbind();
  }
  GL_CALL(glUseProgram(0));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CALL(glBindVertexArray(0));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void api::opengl::OpenGlRenderingPipeline::AddUniform(std::shared_ptr<api::Uniform> uniform) {
  uniforms_.emplace_back(std::dynamic_pointer_cast<api::opengl::OpenGlUniform>(uniform));
}

void api::opengl::OpenGlRenderingPipeline::ViewportChanged(size_t width, size_t height) {
  this->width_ = width;
  this->height_ = height;
}

api::opengl::OpenGlRenderingPipeline::~OpenGlRenderingPipeline() {
  GL_CALL(glDeleteProgram(program_id_));
  GL_CALL(glDisable(GL_DEPTH_TEST));
  GL_CALL(glDisable(GL_CULL_FACE));
}

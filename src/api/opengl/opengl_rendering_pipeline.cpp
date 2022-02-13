//
// Created by artyomd on 1/1/20.
//

#include "src/api/opengl/opengl_rendering_pipeline.hpp"

#include <spdlog/spdlog.h>

#include <utility>

#include "src/api/opengl/opengl_utils.hpp"
#include "src/utils/check.hpp"

static inline void GetProgramInfoLog(GLuint id) {
  int length;
  GL_CALL(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
  if (length != 0) {
    char *message = static_cast<char *>(
        malloc(static_cast<unsigned long>(length) * sizeof(char)));
    GL_CALL(glGetProgramInfoLog(id, length, &length, message));
    spdlog::info("Program creation info message:");
    free(message);
  }
}

api::opengl::OpenGlRenderingPipeline::OpenGlRenderingPipeline(
    std::shared_ptr<Shader> vertex_shader,
    std::shared_ptr<Shader> fragment_shader, api::VertexBufferLayout vbl,
    api::RenderingPipelineConfig config)
    : RenderingPipeline(), vbl_(std::move(vbl)), config_(config) {
  auto gl_vertex_shader =
      std::dynamic_pointer_cast<OpenGlShader>(vertex_shader);
  auto gl_fragment_shader =
      std::dynamic_pointer_cast<OpenGlShader>(fragment_shader);

  GL_CALL(program_id_ = glCreateProgram());
  CHECK(program_id_ != 0, "unable to create a program object")

  GL_CALL(glAttachShader(program_id_, gl_vertex_shader->GetShaderId()));
  GL_CALL(glAttachShader(program_id_, gl_fragment_shader->GetShaderId()));
  GL_CALL(glLinkProgram(program_id_));

  GLint link_status;
  glGetProgramiv(program_id_, GL_LINK_STATUS, &link_status);
  if (link_status == GL_FALSE) {
    GetProgramInfoLog(program_id_);
    GL_CALL(glDetachShader(program_id_, gl_vertex_shader->GetShaderId()));
    GL_CALL(glDetachShader(program_id_, gl_fragment_shader->GetShaderId()));
    throw std::runtime_error("unable to link Gl program");
  }

  GL_CALL(glValidateProgram(program_id_));
  GetProgramInfoLog(program_id_);
  GL_CALL(glDetachShader(program_id_, gl_vertex_shader->GetShaderId()));
  GL_CALL(glDetachShader(program_id_, gl_fragment_shader->GetShaderId()));

  GLint active_uniform_blocks = 0;
  GL_CALL(glGetProgramiv(program_id_, GL_ACTIVE_UNIFORM_BLOCKS,
                         &active_uniform_blocks));

  for (int i = 0; i < active_uniform_blocks; i++) {
    GLint binding_point = -1;
    GLint size_in_bytes = 0;
    GL_CALL(glGetActiveUniformBlockiv(program_id_, i, GL_UNIFORM_BLOCK_BINDING,
                                      &binding_point));
    CHECK(binding_point != -1,
          "can not find uniform block with the given binding point")
    GL_CALL(glGetActiveUniformBlockiv(
        program_id_, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size_in_bytes));
    CHECK(size_in_bytes != 0, "unfirom block size can not be 0")
    auto buffer = std::make_shared<opengl::OpenGlBuffer>(size_in_bytes);
    ubos_.emplace(std::make_pair(binding_point, buffer));
  }
  draw_mode_ = GetGlDrawMode(config_.draw_mode);
}

void api::opengl::OpenGlRenderingPipeline::SetIndexBuffer(
    std::shared_ptr<Buffer> buffer, api::DataType element_type) {
  this->index_buffer_ = std::dynamic_pointer_cast<OpenGlBuffer>(buffer);
  this->index_type_ = api::opengl::GetGlType(element_type);
}

void api::opengl::OpenGlRenderingPipeline::SetVertexBuffer(
    std::shared_ptr<Buffer> buffer) {
  this->vertex_buffer_ = std::dynamic_pointer_cast<OpenGlBuffer>(buffer);
}

void api::opengl::OpenGlRenderingPipeline::Draw(size_t index_count,
                                                size_t offset) {
  GL_CALL(glViewport(0, 0, viewport_width_, viewport_height_));
  GL_CALL(glScissor(0, 0, viewport_width_, viewport_height_));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_->GetBufferId()));
  if (vertex_array_ == 0) {
    GL_CALL(glGenVertexArrays(1, &vertex_array_));
    GL_CALL(glBindVertexArray(vertex_array_));
    const auto &elements = vbl_.GetElements();
    auto stride = static_cast<GLsizei>(vbl_.GetElementSize());
    size_t vertex_offset = 0;
    for (auto element : elements) {
      GL_CALL(glEnableVertexAttribArray(element.binding_index));
      GL_CALL(glVertexAttribPointer(element.binding_index,
                                    static_cast<GLint>(element.count),
                                    GetGlType(element.type), GL_FALSE, stride,
                                    reinterpret_cast<void *>(vertex_offset)));
      vertex_offset +=
          element.count * api::GetDataTypeSizeInBytes(element.type);
    }
  } else {
    GL_CALL(glBindVertexArray(vertex_array_));
  }
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
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, ubo.first,
                             ubo.second->GetBufferId()));
  }
  GL_CALL(glDrawElements(draw_mode_, static_cast<GLsizei>(index_count),
                         index_type_, reinterpret_cast<void *>(offset)));
  for (const auto &ubo : ubos_) {
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, ubo.first, 0));
  }
  for (const auto &texture : textures_) {
    texture.second->Unbind(texture.first);
  }
  GL_CALL(glDisable(GL_DEPTH_TEST));
  GL_CALL(glDisable(GL_CULL_FACE));
  GL_CALL(glUseProgram(0));
  GL_CALL(glBindVertexArray(0));
  GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void api::opengl::OpenGlRenderingPipeline::SetViewPort(uint32_t width,
                                                       uint32_t height) {
  this->viewport_width_ = static_cast<GLsizei>(width);
  this->viewport_height_ = static_cast<GLsizei>(height);
}

void api::opengl::OpenGlRenderingPipeline::UpdateUniformBuffer(
    unsigned int binding_point, void *data) {
  if (ubos_.find(binding_point) != ubos_.end()) {
    ubos_[binding_point]->Update(data);
  }
}

void api::opengl::OpenGlRenderingPipeline::SetTexture(
    unsigned int binding_point, std::shared_ptr<api::Texture2D> texture) {
  textures_.emplace(std::make_pair(
      binding_point,
      std::dynamic_pointer_cast<opengl::OpenglTexture2D>(texture)));
}

api::opengl::OpenGlRenderingPipeline::~OpenGlRenderingPipeline() {
  GL_CALL(glDeleteProgram(program_id_));
  if (vertex_array_ != 0) {
    GL_CALL(glDeleteVertexArrays(1, &vertex_array_));
  }
}

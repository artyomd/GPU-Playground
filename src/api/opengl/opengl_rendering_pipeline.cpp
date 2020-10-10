//
// Created by artyomd on 1/1/20.
//

#include "src/api/opengl/opengl_rendering_pipeline.hpp"

#include <iostream>
#include <utility>

#include "src/api/opengl/opengl_index_buffer.hpp"
#include "src/api/opengl/opengl_rendering_pipeline_layout.hpp"
#include "src/api/opengl/opengl_shader.hpp"
#include "src/api/opengl/opengl_vertex_binding.hpp"
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

api::opengl::OpenGlRenderingPipeline::OpenGlRenderingPipeline(std::shared_ptr<OpenGlRenderingContext> context,
                                                              std::shared_ptr<VertexBinding> vertex_binding,
                                                              std::shared_ptr<IndexBuffer> index_buffer,
                                                              std::shared_ptr<Shader> vertex_shader,
                                                              std::shared_ptr<Shader> fragment_shader,
                                                              std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
                                                              RenderingPipelineLayoutConfig config) :
    RenderingPipeline(std::move(vertex_binding),
                      std::move(index_buffer),
                      std::move(vertex_shader),
                      std::move(fragment_shader),
                      std::move(pipeline_layout),
                      config),
    context_(std::move(context)) {
  DataType type = index_buffer_->GetType();
  if (type != DataType::DATA_TYPE_BYTE &&
      type != DataType::DATA_TYPE_UINT_16 &&
      type != DataType::DATA_TYPE_UINT_32) {
    throw std::runtime_error("unsupported data type");
  }
  GL_CALL(program_id_ = glCreateProgram());
  std::dynamic_pointer_cast<OpenGlShader>(vertex_shader_)->AttachShader(program_id_);
  std::dynamic_pointer_cast<OpenGlShader>(fragment_shader_)->AttachShader(program_id_);
  GL_CALL(glLinkProgram(program_id_));

  GLint link_status;
  glGetProgramiv(program_id_, GL_LINK_STATUS, (int *) &link_status);
  if (link_status == GL_FALSE) {
    GetProgramInfoLog(program_id_);
    std::dynamic_pointer_cast<OpenGlShader>(vertex_shader_)->DetachShader(program_id_);
    std::dynamic_pointer_cast<OpenGlShader>(fragment_shader_)->DetachShader(program_id_);
    throw std::runtime_error("unable to link Gl program");
  }

  GL_CALL(glValidateProgram(program_id_));
  GetProgramInfoLog(program_id_);
  std::dynamic_pointer_cast<OpenGlShader>(vertex_shader_)->DetachShader(program_id_);
  std::dynamic_pointer_cast<OpenGlShader>(fragment_shader_)->DetachShader(program_id_);

  if (config.enable_depth_test) {
    GL_CALL(glEnable(GL_DEPTH_TEST));
    switch (config.depth_function) {
      case DepthFunction::LESS:GL_CALL(glDepthFunc(GL_LESS));
        break;
      default: throw std::runtime_error("not implemented");
    }
  } else {
    glDisable(GL_DEPTH_TEST);
  }
  GL_CALL(glEnable(GL_CULL_FACE));
  switch (config.cull_mode) {
    case CullMode::BACK: GL_CALL(glCullFace(GL_BACK));
      break;
    case CullMode::FRONT: GL_CALL(glCullFace(GL_FRONT));
      break;
    case CullMode::FRONT_AND_BACK: GL_CALL(glCullFace(GL_FRONT_AND_BACK));
      break;
    case CullMode::NONE: GL_CALL(glDisable(GL_CULL_FACE));
      break;
    default: throw std::runtime_error("not implemented");
  }
  switch (config.front_face) {
    case FrontFace::CW:GL_CALL(glFrontFace(GL_CW));
      break;
    case FrontFace::CCW:GL_CALL(glFrontFace(GL_CCW));
      break;
    default: throw std::runtime_error("not implemented");
  }
}

void api::opengl::OpenGlRenderingPipeline::Render() {
  GL_CALL(glUseProgram(program_id_));
  std::dynamic_pointer_cast<OpenGlVertexBinding>(vertex_binding_)->Bind();
  index_buffer_->Bind();
  pipeline_layout_->Bind();

  GL_CALL(glDrawElements(GL_TRIANGLES, index_buffer_->GetCount(), GetGlType(index_buffer_->GetType()), nullptr));

  std::dynamic_pointer_cast<OpenGlRenderingPipelineLayout>(pipeline_layout_)->UnBind();

  index_buffer_->Unbind();
  std::dynamic_pointer_cast<OpenGlVertexBinding>(vertex_binding_)->Unbind();
  GL_CALL(glUseProgram(0));
}

void api::opengl::OpenGlRenderingPipeline::ViewportChanged() {
  context_->WaitForGpuIdle();
  GL_CALL(glViewport(0, 0, context_->GetViewportWidth(), context_->GetViewportHeight()));
}

api::opengl::OpenGlRenderingPipeline::~OpenGlRenderingPipeline() {
  GL_CALL(glDeleteProgram(program_id_));
  GL_CALL(glDisable(GL_DEPTH_TEST));
  GL_CALL(glDisable(GL_CULL_FACE));
}

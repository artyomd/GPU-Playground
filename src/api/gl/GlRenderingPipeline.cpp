//
// Created by artyomd on 1/1/20.
//

#include <iostream>
#include "GlRenderingPipeline.h"
#include "GlIndexBuffer.h"
#include "GlVertexBinding.h"
#include "GlShader.h"
#include "GlUtils.h"
#include "GlRenderingPipelineLayout.h"

namespace api {
static inline void GetProgramInfoLog(int id) {
  int length;
  GL_CALL(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
  if (length!=0) {
    char *message = (char *) malloc(length*sizeof(char));
    GL_CALL(glGetProgramInfoLog(id, length, &length, message));
    std::cout << "Program creation info message:" << message;
    free(message);
  }
}

GlRenderingPipeline::GlRenderingPipeline(const GlRenderingContext *context,
                                         const VertexBinding *vertex_binding, const IndexBuffer *index_buffer,
                                         const Shader *vertex_shader, const Shader *fragment_shader,
                                         const RenderingPipelineLayout *pipeline_layout,
                                         const RenderingPipelineLayoutConfig &config)
    : RenderingPipeline(vertex_binding, index_buffer, vertex_shader, fragment_shader, pipeline_layout, config),
      context_(context) {
  DataType type = index_buffer->GetType();
  if (type!=DATA_TYPE_BYTE &&
      type!=DATA_TYPE_UINT_16 &&
      type!=DATA_TYPE_UINT_32) {
    throw std::runtime_error("unsupported data type");
  }
  GL_CALL(program_id_ = glCreateProgram());
  dynamic_cast<const GlShader *>(vertex_shader)->AttachShader(program_id_);
  dynamic_cast<const GlShader *>(fragment_shader)->AttachShader(program_id_);
  GL_CALL(glLinkProgram(program_id_));

  GLint link_status;
  glGetProgramiv(program_id_, GL_LINK_STATUS, (int *) &link_status);
  if (link_status==GL_FALSE) {
    GetProgramInfoLog(program_id_);
    dynamic_cast<const GlShader *>(vertex_shader)->DetachShader(program_id_);
    dynamic_cast<const GlShader *>(fragment_shader)->DetachShader(program_id_);
    throw std::runtime_error("unable to link Gl program");
  }

  GL_CALL(glValidateProgram(program_id_));
  GetProgramInfoLog(program_id_);
  dynamic_cast<const GlShader *>(vertex_shader)->DetachShader(program_id_);
  dynamic_cast<const GlShader *>(fragment_shader)->DetachShader(program_id_);

  if (config.enable_depth_test_) {
    GL_CALL(glEnable(GL_DEPTH_TEST));
    switch (config.depth_function_) {
      case DepthFunction::LESS:GL_CALL(glDepthFunc(GL_LESS));
        break;
      default: throw std::runtime_error("not implemented");
    }
  } else {
    glDisable(GL_DEPTH_TEST);
  }
  GL_CALL(glEnable(GL_CULL_FACE));
  switch (config.cull_mode_) {
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
  switch (config.front_face_) {
    case FrontFace::CW:GL_CALL(glFrontFace(GL_CW));
      break;
    case FrontFace::CCW:GL_CALL(glFrontFace(GL_CCW));
      break;
    default: throw std::runtime_error("not implemented");
  }
}

GlRenderingPipeline::~GlRenderingPipeline() {
  GL_CALL(glDeleteProgram(program_id_));
  GL_CALL(glDisable(GL_DEPTH_TEST));
  GL_CALL(glDisable(GL_CULL_FACE));
}

void GlRenderingPipeline::Render() {
  GL_CALL(glUseProgram(program_id_));
  dynamic_cast<const GlVertexBinding *>(vertex_binding_)->Bind();
  index_buffer_->Bind();
  pipeline_layout_->Bind();

  GL_CALL(glDrawElements(GL_TRIANGLES, index_buffer_->GetCount(), GetGlType(index_buffer_->GetType()), nullptr));

  dynamic_cast<const GlRenderingPipelineLayout *>(pipeline_layout_)->UnBind();

  index_buffer_->Unbind();
  dynamic_cast<const GlVertexBinding *>(vertex_binding_)->Unbind();
  GL_CALL(glUseProgram(0));
}
void GlRenderingPipeline::ViewportChanged() {
  context_->WaitForGpuIdle();
  GL_CALL(glViewport(0, 0, context_->GetViewportWidth(), context_->GetViewportHeight()));
}
}
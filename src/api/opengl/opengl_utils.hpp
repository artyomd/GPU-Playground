//
// Created by artyomd on 1/6/20.
//

#pragma once

#include <GL/glew.h>
#include <snowhouse/snowhouse.h>

#include "src/api/data_type.hpp"
#include "src/api/redering_pipeline_config.hpp"
#include "src/api/sampler.hpp"
#include "src/api/shader.hpp"

#define GL_CALL(x) \
  api::opengl::GlClearError();\
  x;\
  AssertThat(api::opengl::GlLogCall(#x, __FILE__, __LINE__), snowhouse::Is().True())

namespace api::opengl {
void GlClearError();

bool GlLogCall(const char *function, const char *file, int line);

GLenum GetGlType(api::DataType type);

GLint GetGlFilter(api::Filter filter);

GLint GetGlAddressMode(api::AddressMode address_mode);

GLint GetShaderGlType(api::ShaderType shader_type);

GLenum GetGlDrawMode(api::DrawMode draw_mode);

GLenum GetGlCullMode(api::CullMode cull_mode);

GLenum GetGlFrontFace(api::FrontFace front_face);

GLenum GetGlCompareOp(api::CompareOp compare_op);

}

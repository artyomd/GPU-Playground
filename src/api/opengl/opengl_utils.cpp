#include "src/api/opengl/opengl_utils.hpp"

#include <spdlog/spdlog.h>

#include "src/utils/compiler.hpp"

void api::opengl::GlClearError() {
  while (glGetError() != GL_NO_ERROR);
}

bool api::opengl::GlLogCall(const char *function, const char *file, int line) {
  GLenum error = glGetError();
  if (UTILS_UNLIKELY(error != GL_NO_ERROR)) {
    spdlog::error("[OpenGL error] ({:x}): {} {}:{}", error, function, file, line);
    return false;
  }
  return true;
}

GLenum api::opengl::GetGlType(api::DataType type) {
  switch (type) {
    case api::DataType::BYTE:return GL_UNSIGNED_BYTE;
    case api::DataType::UINT_16:return GL_UNSIGNED_SHORT;
    case api::DataType::UINT_32:return GL_UNSIGNED_INT;
    case api::DataType::FLOAT:return GL_FLOAT;
    default:throw std::runtime_error("unsupported enum");
  }
}

GLint api::opengl::GetShaderGlType(api::ShaderType shader_type) {
  switch (shader_type) {
    case api::ShaderType::VERTEX:return GL_VERTEX_SHADER;
    case api::ShaderType::FRAGMENT:return GL_FRAGMENT_SHADER;
    default:throw std::runtime_error("invalid shader type");
  }
}

GLint api::opengl::GetGlFilter(api::Filter filter) {
  switch (filter) {
    case Filter::LINEAR:return GL_LINEAR;
    case Filter::NEAREST:return GL_NEAREST;
    default:throw std::runtime_error("invalid filter");
  }
}

GLint api::opengl::GetGlAddressMode(api::AddressMode address_mode) {
  switch (address_mode) {
    case api::AddressMode::REPEAT:return GL_REPEAT;
    case api::AddressMode::MIRRORED_REPEAT:return GL_MIRRORED_REPEAT;
    case api::AddressMode::CLAMP_TO_EDGE:return GL_CLAMP_TO_EDGE;
    case api::AddressMode::CLAMP_TO_BORDER:return GL_CLAMP_TO_BORDER;
    default:throw std::runtime_error("invalid address mode");
  }
}

GLenum api::opengl::GetGlDrawMode(api::DrawMode draw_mode) {
  switch (draw_mode) {
    case DrawMode::POINT_LIST:return GL_POINTS;
    case DrawMode::LINE_LIST:return GL_LINES;
    case DrawMode::LINE_STRIP:return GL_LINE_STRIP;
    case DrawMode::TRIANGLE_LIST:return GL_TRIANGLES;
    case DrawMode::TRIANGLE_STRIP:return GL_TRIANGLE_STRIP;
    case DrawMode::TRIANGLE_FAN:return GL_TRIANGLE_FAN;
    default:throw std::runtime_error("invalid draw mode");
  }
}

GLenum api::opengl::GetGlCullMode(api::CullMode cull_mode) {
  switch (cull_mode) {
    case CullMode::NONE:return GL_NONE;
    case CullMode::FRONT:return GL_FRONT;
    case CullMode::BACK:return GL_BACK;
    case CullMode::FRONT_AND_BACK:return GL_FRONT_AND_BACK;
    default:throw std::runtime_error("invalid cull mode");
  }
}
GLenum api::opengl::GetGlFrontFace(api::FrontFace front_face) {
  switch (front_face) {
    case FrontFace::CW:return GL_CW;
    case FrontFace::CCW:return GL_CCW;
    default:throw std::runtime_error("invalid front face");
  }
}

GLenum api::opengl::GetGlCompareOp(api::CompareOp compare_op) {
  switch (compare_op) {
    case CompareOp::NEVER:return GL_NEVER;
    case CompareOp::LESS:return GL_LESS;
    case CompareOp::EQUAL:return GL_EQUAL;
    case CompareOp::LESS_OR_EQUAL:return GL_LEQUAL;
    case CompareOp::GREATER:return GL_GREATER;
    case CompareOp::NOT_EQUAL:return GL_NOTEQUAL;
    case CompareOp::GREATER_OR_EQUAL:return GL_GEQUAL;
    case CompareOp::ALWAYS:return GL_ALWAYS;
    default:throw std::runtime_error("invalid compare op");
  }
}

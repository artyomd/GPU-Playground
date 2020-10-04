//
// Created by artyomd on 1/6/20.
//

#include <GL/glew.h>
#include <vulkan/vulkan.h>
#include <stdexcept>

#include "src/api/Shader.h"

int api::GetShaderGlType(ShaderType shader_type) {
  switch (shader_type) {
    case SHADER_TYPE_VERTEX:return GL_VERTEX_SHADER;
    case SHADER_TYPE_FRAGMENT:return GL_FRAGMENT_SHADER;
  }
  throw std::runtime_error("invalid shader type");
}

VkShaderStageFlags api::GetVkShaderStageFlag(api::ShaderType shader_type) {
  switch (shader_type) {
    case api::SHADER_TYPE_VERTEX:return VK_SHADER_STAGE_VERTEX_BIT;
    case api::SHADER_TYPE_FRAGMENT:return VK_SHADER_STAGE_FRAGMENT_BIT;
  }
}

VkShaderStageFlagBits api::GetShaderVkType(ShaderType shader_type) {
  switch (shader_type) {
    case SHADER_TYPE_VERTEX:return VK_SHADER_STAGE_VERTEX_BIT;
    case SHADER_TYPE_FRAGMENT:return VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  throw std::runtime_error("invalid shader type");
}
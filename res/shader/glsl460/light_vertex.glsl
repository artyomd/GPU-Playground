#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

layout(binding = 0, std140) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
};

layout(location = 0) out vec3 vPos;
layout(location = 1) out vec3 vNormal;

void main() {
  vec4 worldPosition = model * position;
  vPos = worldPosition.xyz;
  vNormal = (model * normal).xyz;
  gl_Position = proj * view * worldPosition;
}

#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 textCoord;

layout(binding = 0, std140) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) out vec2 v_texCoord;

void main() {
    gl_Position = proj * view * model * position;
    v_texCoord = textCoord;
}

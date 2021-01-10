#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textCoords;

layout(binding = 0, std140) uniform UniformBufferObject {
    vec3 max;
    vec3 min;
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) out vec2 v_texCoords;

void main() {
    gl_Position = proj * view * model * position; //vec4((((position-min)*2.)/(max-min))-1, 1.);
    v_texCoords = textCoords;
}
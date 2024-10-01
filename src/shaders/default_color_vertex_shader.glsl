#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec4 position;
layout (location = 1) in uvec4 color;

layout (location = 0) out vec4 v_color;

void main() {
    v_color = vec4(color)/255.0f;
    gl_Position = position;
}

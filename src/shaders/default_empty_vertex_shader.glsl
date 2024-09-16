#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec4 position;

void main() {
    gl_Position = position;
}

#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects: enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_PointSize = 14.0;
    gl_Position = vec4(inPosition.xy, 1.0, 1.0);
    fragColor = inColor.rgb;
}

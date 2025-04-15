#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects: enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    outColor = vec4(fragColor, 0.5 - length(coord));
}

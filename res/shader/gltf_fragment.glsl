#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

precision highp float;

layout(location = 0) in vec3 v_position;
layout(location = 5) in vec2 v_texCoord_0;
layout(location = 6) in vec2 v_texCoord_1;

layout(binding = 1) uniform sampler2D u_texture;

layout(location = 0) out vec4 out_color;

void main(){
    out_color = texture(u_texture, v_texCoord_0);
}

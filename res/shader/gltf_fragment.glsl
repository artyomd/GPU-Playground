#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 v_texCoords;
layout(binding = 1) uniform sampler2D u_texture;
layout(location = 0) out vec4 out_color;

void main(){
    out_color = texture(u_texture, v_texCoords);
}
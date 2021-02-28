#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

precision highp float;

const float GAMMA = 2.2;
const float INV_GAMMA = 1.0 / GAMMA;

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 linearTosRGB(vec3 color)
{
    return pow(color, vec3(INV_GAMMA));
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 sRGBToLinear(vec3 srgbIn)
{
    return vec3(pow(srgbIn.xyz, vec3(GAMMA)));
}

vec4 sRGBToLinear(vec4 srgbIn)
{
    return vec4(sRGBToLinear(srgbIn.xyz), srgbIn.w);
}

layout(location = 0) in vec3 v_position;
layout(location = 5) in vec2 v_texCoord_0;
layout(location = 6) in vec2 v_texCoord_1;
layout(location = 7) in vec4 v_color_0;

layout(binding = 1, std140) uniform Material {
    vec4 base_color_factor;
    vec3 emissive_factor;
    float metallic_factor;
    float roughness_factor;
    float alpha_cutoff;
    bool double_sided;
} material;

layout(constant_id = 2) const int base_color_texture_coords_index = -1;
layout(binding = 2) uniform sampler2D u_base_color_texture;

layout(location = 0) out vec4 out_color;

vec2 getCoords(const int index){
    if (index == 0){
        return v_texCoord_0;
    } else if (index == 1){
        return v_texCoord_1;
    } else {
        return vec2(0.0, 0.0);
    }
}
void main(){
    vec4 baseColor = material.base_color_factor;
    if (base_color_texture_coords_index != -1){
        baseColor *= sRGBToLinear(texture(u_base_color_texture, getCoords(base_color_texture_coords_index)));
    }
    baseColor *= v_color_0;
    out_color = baseColor;
}

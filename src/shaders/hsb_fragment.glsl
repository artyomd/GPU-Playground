#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects: enable

#define TWO_PI 6.28318530718

layout (binding = 0, std140) uniform UniformBufferObject {
    vec2 u_screenSize;
    float u_time;
};
layout (origin_upper_left) in vec4 gl_FragCoord;

layout (location = 0) out vec4 out_color;

vec3 hsb2rgb(in vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    rgb = rgb * rgb * (3.0 - 2.0 * rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}

void main() {
    vec2 st = gl_FragCoord.xy / u_screenSize;
    st.y = 1.0 - st.y;
    vec3 color = vec3(0.0);

    // Use polar coordinates instead of cartesian
    vec2 toCenter = vec2(0.5) - st;
    float angle = atan(toCenter.y, toCenter.x);
    float radius = length(toCenter) * 2.0;

    // Map the angle (-PI to PI) to the Hue (from 0 to 1)
    // and the Saturation to the radius
    color = hsb2rgb(vec3((angle / TWO_PI) + 0.5, radius, 1.0));

    out_color = vec4(color, 1.0);
}

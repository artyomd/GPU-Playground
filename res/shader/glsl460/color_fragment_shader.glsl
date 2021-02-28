#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, std140) uniform UniformBufferObject {
    vec2 u_screenSize;
    float u_time;
};

#define PI 3.14159265359

layout(location = 0) out vec4 out_color;

vec3 colorA = vec3(0.149, 0.141, 0.912);
vec3 colorB = vec3(1.000, 0.833, 0.224);

float plot (vec2 st, float pct){
    return smoothstep(pct-0.01, pct, st.y) -
    smoothstep(pct, pct+0.01, st.y);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_screenSize.xy;
    vec3 color = vec3(0.0);

    vec3 pct = vec3(st.x);

    pct.r = smoothstep(0.0, 1.0, st.x);
    pct.g = sin(st.x*PI);
    pct.b = pow(st.x, 0.5);

    color = mix(colorA, colorB, pct);

    color = mix(color, vec3(1.0, 0.0, 0.0), plot(st, pct.r));
    color = mix(color, vec3(0.0, 1.0, 0.0), plot(st, pct.g));
    color = mix(color, vec3(0.0, 0.0, 1.0), plot(st, pct.b));

    out_color = vec4(color, 1.0);
}
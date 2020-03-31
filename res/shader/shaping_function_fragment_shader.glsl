#version 450
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, std140) uniform UniformBufferObject {
    vec2 u_screenSize;
    float u_time;
};

layout(location = 0) out vec4 out_color;

float plot(float y, float pct){
    return smoothstep(pct-0.02, pct, y) -smoothstep(pct, pct+0.02, y);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_screenSize.xy;

    float y = 0.5+sin(u_time+st.x)/2;

    vec3 color = vec3(y);

    float perc = plot(st.y, y);
    color = (1-perc)*color + perc * vec3(0.0f, 1.0f, 0.0f);

    out_color = vec4(color, 1.0);
}

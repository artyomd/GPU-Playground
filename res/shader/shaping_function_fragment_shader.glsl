#version 450

uniform vec2 u_screenSize;
uniform float u_time;

out vec4 out_color;

float plot(float y, float pct){
     return  smoothstep(pct-0.02, pct, y) -smoothstep( pct, pct+0.02, y);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_screenSize.xy;

    float y = st.x;

    vec3 color = vec3(y);

    float perc = plot(st.y, y);
    color = (1-perc)*color + perc * vec3(0.0f, 1.0f, 0.0f);

    out_color = vec4(color, 1.0);
}

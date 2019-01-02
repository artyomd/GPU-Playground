#version 450

uniform vec2 u_screenSize;
uniform float u_time;

out vec4 out_color;

void main(){
    vec2 st = gl_FragCoord.xy/u_screenSize.xy;
    vec3 color = vec3(0.0);

    vec2 bl = step(vec2(0.1),st);
    vec2 tr = step(vec2(0.7),1.0-st);
    float pct = bl.x * bl.y * tr.x * tr.y;

    vec2 bl1 = step(vec2(0.1),(1.0-st));
    vec2 tr1 = step(vec2(0.7), st);
    float pct1 = bl1.x * bl1.y * tr1.x * tr1.y;

    vec2 bl2 = step(vec2(0.1),vec2(1.0-st.x, st.y));
    vec2 tr2 = step(vec2(0.7), vec2(st.x, 1.0-st.y));
    float pct2 = bl2.x * bl2.y * tr2.x * tr2.y;

    vec2 bl3 = step(vec2(0.1),vec2(st.x, 1.0-st.y));
    vec2 tr3 = step(vec2(0.7), vec2(1.0-st.x, st.y));
    float pct3 = bl3.x * bl3.y * tr3.x * tr3.y;


    vec2 bl4 = step(vec2(0.4),st);
    vec2 tr4 = step(vec2(0.4),1.0-st);
    float pct4 = bl4.x * bl4.y * tr4.x * tr4.y;


    color = (vec3(pct + pct1+pct2+pct3+pct4))*vec3(1.0, 0.0, 0.0);

    out_color = vec4(color,1.0);
}
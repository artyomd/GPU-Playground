// Star Nest by Pablo Roman Andrioli

// This content is under the MIT License.

#version 450

#define iterations 17
#define formuparam 0.53

#define volsteps 20
#define stepsize 0.1

#define zoom   0.800
#define tile   0.850
#define speed  0.010

#define brightness 0.0015
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.850

out vec4 out_color;

uniform vec2 u_screenSize;
uniform float u_time;

void main()
{
    //get coords and direction
    vec2 uv=gl_FragCoord.xy/u_screenSize.xy-.5;
    uv.y*=u_screenSize.y/u_screenSize.x;
    vec3 dir=vec3(uv*zoom, 1.);
    float time=u_time*speed+.25;

    vec3 from=vec3(1., .5, 0.5);
    from+=vec3(time*2., time, -2.);

    //volumetric rendering
    float s=0.1, fade=1.;
    vec3 v=vec3(0.);
    for (int r=0; r<volsteps; r++) {
        vec3 p=from+s*dir*.5;
        p = abs(vec3(tile)-mod(p, vec3(tile*2.)));// tiling fold
        float pa, a=pa=0.;
        for (int i=0; i<iterations; i++) {
            p=abs(p)/dot(p, p)-formuparam;// the magic formula
            a+=abs(length(p)-pa);// absolute sum of average change
            pa=length(p);
        }
        float dm=max(0., darkmatter-a*a*.001);//dark matter
        a*=a*a;// add contrast
        if (r>6) fade*=1.-dm;// dark matter, don't render near
        //v+=vec3(dm,dm*.5,0.);
        v+=fade;
        v+=vec3(s, s*s, s*s*s*s)*a*brightness*fade;// coloring based on distance
        fade*=distfading;// distance fading
        s+=stepsize;
    }
    v=mix(vec3(length(v)), v, saturation);//color adjust
    out_color = vec4(v*.01, 1.);

}
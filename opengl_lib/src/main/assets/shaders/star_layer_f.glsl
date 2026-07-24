#version 300 es
precision highp float;
in vec3 v_color;
in float v_twinkleOffset;
out vec4 outColor;
uniform float u_time;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    if (dist > 0.5) discard;
    
    float twinkle = sin(u_time * 2.0 + v_twinkleOffset) * 0.3 + sin(u_time * 4.0 + v_twinkleOffset) * 0.15 + 0.55;
    
    if (twinkle < 0.15) {
        discard;
    }
    
    float core = 1.0 - smoothstep(0.0, 0.25, dist);
    float glow = 1.0 - smoothstep(0.0, 0.5, dist);
    
    float finalAlpha = core * twinkle + glow * twinkle * 0.5;
    outColor = vec4(v_color * twinkle, finalAlpha);
}

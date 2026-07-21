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
    float twinkle = sin(u_time * 2.0 + v_twinkleOffset) * 0.3 + sin(u_time * 5.0 + v_twinkleOffset * 1.7) * 0.2 + 0.5;
    float core = 1.0 - smoothstep(0.0, 0.25, dist);
    float glow = 1.0 - smoothstep(0.0, 0.5, dist);
    float spikeX = exp(-abs(coord.x) * 12.0) * (1.0 - smoothstep(0.0, 0.45, abs(coord.y)));
    float spikeY = exp(-abs(coord.y) * 12.0) * (1.0 - smoothstep(0.0, 0.45, abs(coord.x)));
    float spikes = (spikeX + spikeY) * 0.35;
    float finalAlpha = core * twinkle + glow * twinkle * 0.4 + spikes * twinkle;
    outColor = vec4(v_color * twinkle, finalAlpha);
}

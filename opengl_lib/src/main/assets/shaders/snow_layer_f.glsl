#version 300 es
precision highp float;
out vec4 outColor;
uniform float u_snowIntensity;
uniform float u_snowShape;
void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    if (dist > 0.45) discard;
    float alpha;
    if (u_snowShape < 0.5) {
        alpha = smoothstep(0.45, 0.0, dist);
    } else {
        float angle = atan(coord.y, coord.x);
        float r = 0.38 + 0.08 * cos(6.0 * angle);
        alpha = smoothstep(r + 0.05, r - 0.05, dist);
    }
    alpha *= u_snowIntensity;
    outColor = vec4(0.85, 0.88, 0.92, alpha);
}

#version 300 es
precision mediump float;
in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;
void main() {
    vec2 centered = vTexCoord - vec2(0.5);
    float dist = length(centered);
    if (dist > 0.5) discard;
    float alpha = 1.0 - smoothstep(0.4, 0.5, dist);
    vec2 normalDir = (dist > 0.001) ? normalize(centered) : vec2(0.0);
    float normalLen = dist / 0.5;
    vec2 normal = normalDir * normalLen;
    fragColor = vec4(normal * 0.5 + 0.5, 0.0, alpha);
}

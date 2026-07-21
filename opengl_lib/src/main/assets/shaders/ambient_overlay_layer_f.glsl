#version 300 es
precision highp float;
layout(location = 0) out vec4 outColor;
uniform float u_flashIntensity;
uniform bool u_isNight;

void main() {
    vec3 flashColor = u_isNight ? vec3(0.25, 0.35, 0.55) : vec3(0.4, 0.5, 0.65);
    float intensity = u_flashIntensity * 0.18;
    vec3 result = flashColor * intensity;
    result = clamp(result, 0.0, 1.0);
    outColor = vec4(result, intensity);
}

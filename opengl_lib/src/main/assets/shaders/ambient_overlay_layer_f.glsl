#version 300 es
precision highp float;
layout (location = 0) out vec4 outColor;
uniform float u_flashIntensity;
uniform bool u_isNight;
uniform int u_overlayMode;
uniform float u_fogIntensity;
uniform vec3 u_overlayColor;

in vec2 v_texCoord;

void main() {
    if (u_overlayMode == 1) {
        vec3 fogColor = u_isNight ? vec3(0.18, 0.20, 0.25) : vec3(0.80, 0.82, 0.80);
        float y = v_texCoord.y;
        float alpha = smoothstep(1.0, 0.0, y) * u_fogIntensity;
        outColor = vec4(fogColor, alpha);
    } else if (u_overlayMode == 2) {
        float y = v_texCoord.y;
        float alpha = smoothstep(1.0, 0.0, y) * u_fogIntensity;
        outColor = vec4(u_overlayColor, alpha);
    } else if (u_overlayMode == 3) {
        float y = v_texCoord.y;
        float alpha = smoothstep(1.0, 0.0, y) * u_fogIntensity;
        outColor = vec4(u_overlayColor, alpha);
    } else {
        vec3 flashColor = u_isNight ? vec3(0.25, 0.35, 0.55) : vec3(0.4, 0.5, 0.65);
        float intensity = u_flashIntensity * 0.18;
        vec3 result = flashColor * intensity;
        result = clamp(result, 0.0, 1.0);
        outColor = vec4(result, intensity);
    }
}

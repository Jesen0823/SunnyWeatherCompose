#version 300 es
precision highp float;
layout (location = 0) out vec4 outColor;
uniform float u_flashIntensity;
uniform bool u_isNight;
uniform bool u_isFogMode;
uniform float u_fogIntensity;

in vec2 v_texCoord;

void main() {
    if (u_isFogMode) {
        vec3 fogColor = u_isNight ? vec3(0.18, 0.20, 0.25) : vec3(0.80, 0.82, 0.80);
        float y = v_texCoord.y;
        float alpha = smoothstep(1.0, 0.0, y) * u_fogIntensity;
        outColor = vec4(fogColor, alpha);
    } else {
        vec3 flashColor = u_isNight ? vec3(0.25, 0.35, 0.55) : vec3(0.4, 0.5, 0.65);
        float intensity = u_flashIntensity * 0.18;
        vec3 result = flashColor * intensity;
        result = clamp(result, 0.0, 1.0);
        outColor = vec4(result, intensity);
    }
}

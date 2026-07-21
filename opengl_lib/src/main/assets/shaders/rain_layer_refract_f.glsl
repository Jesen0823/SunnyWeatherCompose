#version 300 es
precision mediump float;
in vec2 vTexCoord;
layout (location = 0) out vec4 fragColor;
uniform sampler2D uBackground;
uniform sampler2D uDistort;
uniform float uRefraction;
uniform float uTime;

void main() {
    vec4 distort = texture(uDistort, vTexCoord);
    float mask = distort.a;
    if (mask < 0.01) {
        fragColor = texture(uBackground, vTexCoord);
        return;
    }
    vec2 normal = distort.xy * 2.0 - 1.0;
    float normalLen = length(normal);
    vec2 normalDir = (normalLen > 0.001) ? normalize(normal) : vec2(0.0);
    float refractionScale = uRefraction * normalLen * 0.4;
    vec2 refractDir = -normalDir * refractionScale;
    vec3 colorR = texture(uBackground, vTexCoord + refractDir * 1.15).rgb;
    vec3 colorG = texture(uBackground, vTexCoord + refractDir * 1.0).rgb;
    vec3 colorB = texture(uBackground, vTexCoord + refractDir * 0.85).rgb;
    vec3 chromaticColor = vec3(colorR.r, colorG.g, colorB.b);
    float fresnel = pow(normalLen, 3.0);
    chromaticColor = mix(chromaticColor, vec3(0.85, 0.88, 0.92), fresnel * 0.6);
    float edgeHighlight = smoothstep(0.7, 0.95, normalLen) * mask;
    chromaticColor = mix(chromaticColor, vec3(0.95, 0.96, 0.98), edgeHighlight * 0.6);
    float centerLens = (1.0 - normalLen) * mask * 0.1;
    chromaticColor += vec3(centerLens);
    fragColor = vec4(chromaticColor, 1.0);
}

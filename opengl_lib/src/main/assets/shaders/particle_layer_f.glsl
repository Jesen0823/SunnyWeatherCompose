#version 300 es
precision highp float;
precision highp int;
layout (location = 0) out vec4 outColor;
uniform float u_time;
uniform vec2 u_screenSize;
uniform int u_particleType;
uniform float u_particleDensity;
uniform vec3 u_particleColor;
uniform float u_particleVisibility;

vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec2 p) {
    const float K1 = 0.366025404;
    const float K2 = 0.211324865;
    vec2 i = floor(p + (p.x + p.y) * K1);
    vec2 a = p - i + (i.x + i.y) * K2;
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0 * K2;
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    vec3 n = h * h * h * h * vec3(dot(a, hash(i)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, vec3(70.0));
}

float fbm(vec2 n) {
    float total = 0.0, amplitude = 0.5;
    for (int i = 0; i < 5; i++) {
        total += noise(n) * amplitude;
        n *= 2.0;
        amplitude *= 0.5;
    }
    return total;
}

void main() {
    vec2 p = gl_FragCoord.xy / u_screenSize.xy;
    vec2 uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);
    vec3 result = vec3(0.0);
    float alpha = 0.0;

    float speed = 0.02;
    if (u_particleType == 3) {
        speed = 0.05;
    }

    float particleNoise = fbm(uv * 5.0 + vec2(u_time * speed, 0.0));
    float particle = smoothstep(0.3, 0.7, particleNoise);
    particle *= u_particleDensity;

    if (u_particleType == 0) {
        alpha = particle * (1.0 - u_particleVisibility) * 0.8;
    } else if (u_particleType == 1) {
        float fogDensity = 1.0 - smoothstep(0.0, 1.0, p.y);
        alpha = particle * fogDensity * (1.0 - u_particleVisibility);
    } else if (u_particleType == 2) {
        alpha = particle * (1.0 - u_particleVisibility) * 0.6;
    } else {
        float sandNoise = fbm(uv * 3.0 + vec2(u_time * speed * 2.0, 0.0));
        float sand = smoothstep(0.2, 0.8, sandNoise);
        alpha = sand * u_particleDensity * (1.0 - u_particleVisibility) * 0.7;
    }

    vec2 center = vec2(0.5);
    float distToCenter = length(p - center);
    float maxDist = length(vec2(0.5));
    float radialBlur = smoothstep(maxDist * 0.8, maxDist * 1.1, distToCenter);
    radialBlur = 1.0 - radialBlur;

    float edgeBlurX = smoothstep(0.0, 0.15, p.x) * smoothstep(0.85, 1.0, p.x);
    float edgeBlurY = smoothstep(0.0, 0.15, p.y) * smoothstep(0.85, 1.0, p.y);
    float combinedEdgeBlur = edgeBlurX * edgeBlurY * 0.6 + radialBlur * 0.4;
    alpha *= combinedEdgeBlur;

    result = u_particleColor * alpha;
    alpha = clamp(alpha, 0.0, 1.0);
    outColor = vec4(result, alpha);
}

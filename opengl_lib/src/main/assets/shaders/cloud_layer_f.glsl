#version 300 es
precision highp float;
layout (location = 0) out vec4 outColor;
uniform float u_time;
uniform vec2 u_screenSize;
uniform float u_cloudCoverage;
uniform float u_cloudDarkness;
uniform float u_cloudLightness;
uniform float u_cloudSpeed;
uniform float u_cloudScale;
uniform float u_cloudAlpha;
uniform int u_isNight;
uniform int u_cloudMode;
const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);

vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}
float noise(in vec2 p) {
    const float K1 = 0.366025404;
    const float K2 = 0.211324865;
    vec2 i = floor(p + (p.x + p.y) * K1);
    vec2 a = p - i + (i.x + i.y) * K2;
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0 * K2;
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    vec3 n = h * h * h * h * vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, vec3(70.0));
}
float fbm(vec2 n) {
    float total = 0.0, amplitude = 0.1;
    for (int i = 0; i < 7; i++) {
        total += noise(n) * amplitude;
        n = m * n;
        amplitude *= 0.4;
    }
    return total;
}
void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 p = fragCoord.xy / u_screenSize.xy;
    vec2 uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);
    float time = u_time * u_cloudSpeed;
    float q = fbm(uv * u_cloudScale * 0.5);
    float r = 0.0;
    uv *= u_cloudScale;
    uv -= q - time;
    float weight = 0.8;
    for (int i = 0; i < 8; i++) {
        r += abs(weight * noise(uv));
        uv = m * uv + time;
        weight *= 0.7;
    }
    float f = 0.0;
    uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);
    uv *= u_cloudScale;
    uv -= q - time;
    weight = 0.7;
    for (int i = 0; i < 8; i++) {
        f += weight * noise(uv);
        uv = m * uv + time;
        weight *= 0.6;
    }
    f *= r + f;
    float c = 0.0;
    time = u_time * u_cloudSpeed * 2.0;
    uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);
    uv *= u_cloudScale * 2.0;
    uv -= q - time;
    weight = 0.4;
    for (int i = 0; i < 7; i++) {
        c += weight * noise(uv);
        uv = m * uv + time;
        weight *= 0.6;
    }
    float c1 = 0.0;
    time = u_time * u_cloudSpeed * 3.0;
    uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);
    uv *= u_cloudScale * 3.0;
    uv -= q - time;
    weight = 0.4;
    for (int i = 0; i < 7; i++) {
        c1 += abs(weight * noise(uv));
        uv = m * uv + time;
        weight *= 0.6;
    }
    c += c1;
    vec3 dayCloudBase = vec3(0.88, 0.90, 0.96);
    vec3 snowCloudBase = vec3(0.92, 0.93, 0.96);
    vec3 cloudColour = mix(dayCloudBase, snowCloudBase, float(u_cloudMode == 2));
    cloudColour = mix(cloudColour, vec3(1.0, 1.0, 1.0), clamp(c * 1.0, 0.0, 1.0));

    cloudColour = mix(cloudColour, vec3(0.3, 0.3, 0.35), u_cloudDarkness);
    cloudColour *= (1.0 + u_cloudLightness * 0.3);

    f = u_cloudCoverage + u_cloudAlpha * f * r;
    float cloudAlpha = clamp(f + c, 0.0, 1.0);
    float nightAlpha = u_isNight == 1 ? 0.85 : 1.0;
    cloudAlpha *= nightAlpha;
    
    vec3 nightTint = mix(vec3(0.15, 0.18, 0.25), vec3(0.35, 0.38, 0.48), c);
    vec3 snowNightTint = mix(vec3(0.25, 0.28, 0.32), vec3(0.45, 0.48, 0.52), c);
    vec3 currentNightTint = mix(nightTint, snowNightTint, float(u_cloudMode == 2));
    cloudColour = mix(cloudColour, currentNightTint, float(u_isNight));
    
    float snowBrightness = u_cloudMode == 2 ? (1.0 - u_cloudDarkness * 0.3) : 1.0;
    cloudColour *= snowBrightness;
    cloudColour = clamp(cloudColour, 0.0, 1.0);
    
    outColor = vec4(cloudColour, cloudAlpha);
}

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
uniform vec2 u_moonPos;
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
    int iterations = u_cloudMode == 0 ? 5 : 4;
    for (int i = 0; i < iterations; i++) {
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
    
    float verticalGradient = 0.3 + p.y * 0.7;
    if (u_cloudMode == 1) {
        verticalGradient = 1.0 - p.y * 0.7;
        float bottomCutoff = smoothstep(0.5, 0.85, p.y);
        if (bottomCutoff > 0.9) {
            discard;
        }
    }
    
    if (verticalGradient < 0.05) {
        discard;
    }
    
    vec3 dayCloudBase = vec3(0.88, 0.90, 0.96);
    vec3 snowCloudBase = vec3(0.92, 0.93, 0.96);
    vec3 rainCloudBase = vec3(0.80, 0.82, 0.86);
    vec3 cloudColour = mix(dayCloudBase, snowCloudBase, float(u_cloudMode == 2));
    cloudColour = mix(cloudColour, rainCloudBase, float(u_cloudMode == 1));
    
    float time = u_time * u_cloudSpeed;
    float q = fbm(uv * u_cloudScale * 0.5);
    
    float r = 0.0;
    vec2 uv_r = uv * u_cloudScale;
    uv_r -= q - time;
    float weight = 0.8;
    int noiseIterations = u_cloudMode == 0 ? 6 : 5;
    for (int i = 0; i < noiseIterations; i++) {
        r += abs(weight * noise(uv_r));
        uv_r = m * uv_r + time;
        weight *= 0.7;
    }
    
    float f = 0.0;
    vec2 uv_f = uv * u_cloudScale;
    uv_f -= q - time;
    weight = 0.7;
    for (int i = 0; i < noiseIterations; i++) {
        f += weight * noise(uv_f);
        uv_f = m * uv_f + time;
        weight *= 0.6;
    }
    f *= r + f;
    
    float c = 0.0;
    float time_c = u_time * u_cloudSpeed * 2.0;
    vec2 uv_c = uv * u_cloudScale * 2.0;
    uv_c -= q - time_c;
    weight = 0.4;
    for (int i = 0; i < 5; i++) {
        c += weight * noise(uv_c);
        uv_c = m * uv_c + time_c;
        weight *= 0.6;
    }
    
    float c1 = 0.0;
    float time_c1 = u_time * u_cloudSpeed * 3.0;
    vec2 uv_c1 = uv * u_cloudScale * 3.0;
    uv_c1 -= q - time_c1;
    weight = 0.4;
    for (int i = 0; i < 5; i++) {
        c1 += abs(weight * noise(uv_c1));
        uv_c1 = m * uv_c1 + time_c1;
        weight *= 0.6;
    }
    c += c1;
    
    float cloudAlpha = 0.0;
    
    if (u_cloudMode == 1) {
        float baseCoverage = u_cloudCoverage;
        float threshold = 0.35 - baseCoverage * 0.20;
        
        float cloudNoise = f * 0.5 + r * 0.35 + c * 0.15;
        float shape = smoothstep(threshold, threshold + 0.4, cloudNoise);
        
        float density = u_cloudAlpha * 0.12;
        float baseAlpha = clamp(shape * density + c * 0.4, 0.0, 1.0);
        
        float layer1 = smoothstep(threshold, threshold + 0.35, f * 0.5 + r * 0.5);
        float layer2 = smoothstep(threshold + 0.05, threshold + 0.4, c * 0.5 + r * 0.5);
        float layer3 = smoothstep(threshold + 0.1, threshold + 0.45, noise(uv * u_cloudScale * 4.0 + time));
        
        cloudAlpha = baseAlpha + layer1 * 0.3 + layer2 * 0.2 + layer3 * 0.15;
        cloudAlpha = clamp(cloudAlpha, 0.0, 1.0);
        
        cloudAlpha *= verticalGradient;
        
        float gapNoise = smoothstep(0.3, 0.7, noise(uv * 2.0 + time * 0.2));
        cloudAlpha *= (1.0 - gapNoise * 0.3);
    } else {
        cloudColour = mix(cloudColour, vec3(1.0, 1.0, 1.0), clamp(c * 0.8, 0.0, 1.0));
        
        f = u_cloudCoverage + u_cloudAlpha * f * r;
        cloudAlpha = clamp(f + c, 0.0, 1.0);
        
        cloudAlpha *= verticalGradient;
    }
    
    float nightAlpha = u_isNight == 1 ? 0.85 : 1.0;
    cloudAlpha *= nightAlpha;
    
    if (cloudAlpha < 0.005) {
        discard;
    }
    
    cloudColour = mix(cloudColour, vec3(0.25, 0.28, 0.32), u_cloudDarkness);
    cloudColour *= (1.0 + u_cloudLightness * 0.3);
    
    vec3 nightTint = mix(vec3(0.15, 0.18, 0.25), vec3(0.35, 0.38, 0.48), cloudAlpha);
    vec3 snowNightTint = mix(vec3(0.25, 0.28, 0.32), vec3(0.45, 0.48, 0.52), cloudAlpha);
    vec3 rainNightTint = mix(vec3(0.12, 0.14, 0.20), vec3(0.25, 0.28, 0.35), cloudAlpha);
    vec3 currentNightTint = mix(nightTint, snowNightTint, float(u_cloudMode == 2));
    currentNightTint = mix(currentNightTint, rainNightTint, float(u_cloudMode == 1));
    cloudColour = mix(cloudColour, currentNightTint, float(u_isNight));
    
    float snowBrightness = u_cloudMode == 2 ? (1.0 - u_cloudDarkness * 0.3) : 1.0;
    cloudColour *= snowBrightness;
    cloudColour = clamp(cloudColour, 0.0, 1.0);
    
    if (u_isNight == 1) {
        float moonDist = distance(p, u_moonPos);
        
        if (moonDist > 0.5) {
            outColor = vec4(cloudColour, cloudAlpha);
            return;
        }
        
        float moonInfluence = smoothstep(0.40, 0.0, moonDist);
        
        float moonGlowFactor = smoothstep(0.18, 0.0, moonDist);
        float moonCoreFactor = smoothstep(0.085, 0.0, moonDist);
        
        float cloudThickness = clamp(cloudAlpha * 1.8, 0.0, 1.0);
        
        float randomFactor = fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
        randomFactor = randomFactor * 0.6 + 0.7;
        
        float opacityReduction = 0.0;
        if (moonDist < 0.18) {
            float coreReduction = moonCoreFactor * (1.0 - cloudThickness * 0.4);
            float glowReduction = moonGlowFactor * (1.0 - cloudThickness * 0.7);
            opacityReduction = mix(glowReduction, coreReduction, moonCoreFactor);
            opacityReduction *= randomFactor;
        }
        
        cloudAlpha *= (1.0 - opacityReduction * 0.8);
        
        float moonGlowStrength = moonGlowFactor * (1.0 - cloudThickness * 0.5);
        vec3 moonGlowColor = vec3(0.92, 0.94, 0.99);
        cloudColour = mix(cloudColour, moonGlowColor, moonGlowStrength * 0.4);
        
        float moonLight = moonInfluence * (1.0 - cloudThickness * 0.6);
        cloudColour = cloudColour + moonGlowColor * moonLight * 0.25 * randomFactor;
    }
    
    outColor = vec4(cloudColour, cloudAlpha);
}

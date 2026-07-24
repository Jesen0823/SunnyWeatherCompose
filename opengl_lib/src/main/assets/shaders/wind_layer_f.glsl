#version 300 es
precision highp float;
layout (location = 0) out vec4 outColor;
uniform float u_time;
uniform vec2 u_screenSize;
uniform bool u_windLinesEnabled;
uniform float u_windStrength;

float hash(float x) {
    return fract(sin(x) * 43758.5453123);
}

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453123);
}

float windLineLayer(vec2 p, float time, float strength, float scale, float speedMultiplier, float lineWidth, float alphaMultiplier) {
    float lineCount = 6.0 + strength * 8.0;
    float lineSpeed = 0.4 + strength * 0.8 * speedMultiplier;
    float width = lineWidth + strength * 0.0025;
    float length = 0.18 + strength * 0.22;
    float result = 0.0;

    for (float i = 0.0; i < lineCount; i += 1.0) {
        float baseY = 0.96 - i * (0.92 / lineCount);
        float speedOffset = hash(i * 7.3) * 0.5 + 0.5;
        float timeOffset = time * lineSpeed * speedOffset;

        float startX = mod(1.5 + i * 0.18 - timeOffset, 2.0) - 0.3;
        float startY = mod(baseY - timeOffset * 0.7 + 1.0, 1.1) - 0.05;

        float lineAngle = radians(145.0);
        float c = cos(lineAngle);
        float s = sin(lineAngle);
        
        c = -0.8192;
        s = -0.5736;

        vec2 delta = p - vec2(startX, startY);
        float alignedX = delta.x * c + delta.y * s;
        float alignedY = -delta.x * s + delta.y * c;

        float distX = clamp(alignedX, 0.0, length) / length;
        float distY = abs(alignedY) / width;
        float dist = max(distX, distY);
        float line = smoothstep(0.0, 0.25, 1.0 - dist) * smoothstep(1.0, 0.45, 1.0 - dist);
        line *= line;

        float gapOffset = hash(i * 123.456 + time * 0.04);
        float gapPos = mod(alignedX * 7.0 + gapOffset, 0.22);
        float gap = smoothstep(0.09, 0.12, gapPos) * smoothstep(0.22, 0.19, gapPos);
        line *= gap;

        float fadeY = smoothstep(0.0, 0.15, p.y) * smoothstep(1.0, 0.85, p.y);
        line *= fadeY;

        float speedFade = smoothstep(0.0, 0.35, speedOffset);
        line *= speedFade;

        float noiseWobble = sin(time * 2.5 + i * 1.5) * 0.012;
        vec2 wobbleDelta = delta + vec2(noiseWobble * c, noiseWobble * s);
        float wobbleAlignedX = wobbleDelta.x * c + wobbleDelta.y * s;
        float wobbleDistX = clamp(wobbleAlignedX, 0.0, length) / length;
        float wobbleDist = max(wobbleDistX, distY);
        float wobbleLine = smoothstep(0.0, 0.25, 1.0 - wobbleDist) * smoothstep(1.0, 0.45, 1.0 - wobbleDist);
        line += wobbleLine * 0.35;

        result += line;
    }

    return result * alphaMultiplier;
}

float windParticle(vec2 p, float time, float strength) {
    float particleCount = 35.0 + strength * 50.0;
    float speed = 0.35 + strength * 0.6;
    float size = 0.0025 + strength * 0.0035;
    float result = 0.0;

    for (float i = 0.0; i < particleCount; i += 1.0) {
        float seed = i * 137.5;
        float startX = hash(seed) * 1.9 - 0.45;
        float startY = hash(seed + 1.0) * 1.0;
        float x = mod(startX - time * speed + hash(seed + 2.0) * 0.035 * sin(time * 0.5 + seed), 1.9) - 0.45;
        float y = mod(startY + time * speed * 0.7 + sin(time * 0.3 + seed) * 0.09, 1.1) - 0.05;
        float dist = length(p - vec2(x, y));
        float particle = smoothstep(size, 0.0, dist);
        float fadeY = smoothstep(0.0, 0.12, y) * smoothstep(1.0, 0.88, y);
        float alpha = hash(seed + 3.0) * 0.6 + 0.35;
        result += particle * fadeY * alpha;
    }

    return result;
}

void main() {
    vec2 p = gl_FragCoord.xy / u_screenSize.xy;
    vec3 result = vec3(0.0);
    float alpha = 0.0;

    if (u_windLinesEnabled) {
        float windFar = windLineLayer(p, u_time, u_windStrength, 1.0, 0.55, 0.0012, 0.45);
        float windMid = windLineLayer(p, u_time * 1.25, u_windStrength, 1.35, 1.0, 0.002, 0.65);
        float windNear = windLineLayer(p, u_time * 1.6, u_windStrength, 1.75, 1.45, 0.0035, 0.8);

        float particles = windParticle(p, u_time, u_windStrength);

        vec3 windColor = vec3(0.70, 0.75, 0.80);
        vec3 particleColor = vec3(0.78, 0.82, 0.88);

        result += windColor * (windFar + windMid + windNear);
        result += particleColor * particles;

        alpha += (windFar + windMid + windNear) * (0.10 + u_windStrength * 0.12);
        alpha += particles * (0.05 + u_windStrength * 0.07);
    }

    alpha = clamp(alpha, 0.0, 0.4);
    outColor = vec4(result, alpha);
}
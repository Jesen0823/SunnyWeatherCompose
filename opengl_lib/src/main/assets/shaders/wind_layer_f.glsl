#version 300 es
precision highp float;
layout(location = 0) out vec4 outColor;
uniform float u_time;
uniform vec2 u_screenSize;
uniform bool u_windLinesEnabled;
uniform float u_windStrength;

float hash(float x) {
    return fract(sin(x) * 43758.5453123);
}

float windLine(vec2 p, float time, float strength) {
    float lineCount = 3.0 + strength * 4.0;
    float lineSpeed = 0.3 + strength * 0.5;
    float lineWidth = 0.001 + strength * 0.0015;
    float lineLength = 0.15 + strength * 0.15;
    float slope = 0.3 + strength * 0.2;
    float result = 0.0;

    for (float i = 0.0; i < lineCount; i += 1.0) {
        float baseY = 0.1 + i * 0.22;
        float baseX = mod(time * lineSpeed + i * 0.3, 1.5) - 0.25;
        float y = p.y;
        float x = p.x - (y - baseY) * slope;
        float distX = abs(x - baseX) / lineLength;
        float distY = abs(y - baseY) / lineWidth;
        float dist = max(distX, distY);
        float line = smoothstep(0.0, 1.0, 1.0 - dist);
        line *= line;

        float gapOffset = hash(i * 123.456) * 0.5;
        float gapPos = mod(x + gapOffset, 0.12);
        float gap = smoothstep(0.05, 0.07, gapPos) *
                    smoothstep(0.12, 0.10, gapPos);
        line *= gap;

        float fadeY = smoothstep(0.0, 0.3, y) * smoothstep(1.0, 0.7, y);
        line *= fadeY;

        result += line;
    }

    return result;
}

void main() {
    vec2 p = gl_FragCoord.xy / u_screenSize.xy;
    vec3 result = vec3(0.0);
    float alpha = 0.0;

    if (u_windLinesEnabled) {
        float wind = windLine(p, u_time, u_windStrength);
        result += vec3(0.65, 0.70, 0.75) * wind;
        alpha += wind * (0.03 + u_windStrength * 0.05);
    }

    alpha = clamp(alpha, 0.0, 0.2);
    outColor = vec4(result, alpha);
}

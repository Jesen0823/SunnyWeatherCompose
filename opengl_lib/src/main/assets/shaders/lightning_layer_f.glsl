#version 300 es
precision highp float;
layout(location = 0) out vec4 outColor;
uniform float u_time;
uniform vec2 u_screenSize;
uniform float u_flashIntensity;
uniform bool u_isNight;

float rand(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float sdSegment(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    float t = clamp(dot(p - a, ab) / dot(ab, ab), 0.0, 1.0);
    return length(p - a - ab * t);
}

float getLightningValue(vec2 uv, vec2 start, vec2 end, float width) {
    float dist = sdSegment(uv, start, end);
    float core = smoothstep(width * 0.2, 0.0, dist) * 1.5;
    float inner = smoothstep(width * 0.5, 0.0, dist);
    float outer = smoothstep(width * 1.2, 0.0, dist);
    float glow = smoothstep(width * 4.0, 0.0, dist);
    inner *= (1.0 - core);
    outer *= (1.0 - inner);
    glow *= (1.0 - outer);
    return core + inner * 0.8 + outer * 0.4 + glow * 0.2;
}

void main() {
    vec2 uv = gl_FragCoord.xy / u_screenSize.xy;
    vec3 col = vec3(0.0);
    float alpha = 0.0;

    float flash = u_flashIntensity;

    if (flash < 0.001) {
        outColor = vec4(col, alpha);
        return;
    }

    float seed = floor(u_time * 500.0);
    float mainX = 0.1 + rand(vec2(seed)) * 0.8;
    float widthVar = 0.5 + rand(vec2(seed + 100.0)) * 0.5;
    float brightnessVar = 0.5 + rand(vec2(seed + 200.0)) * 0.5;
    float heightVar = 0.4 + rand(vec2(seed + 300.0)) * 0.6;

    float totalFlash = flash * brightnessVar;
    float baseWidth = 0.015 * widthVar;

    vec3 coreCol = vec3(1.0, 1.0, 0.95);
    vec3 innerCol = u_isNight ? vec3(0.85, 0.92, 1.0) : vec3(0.96, 0.98, 1.0);
    vec3 outerCol = u_isNight ? vec3(0.6, 0.75, 1.0) : vec3(0.85, 0.9, 0.98);
    vec3 glowCol = u_isNight ? vec3(0.4, 0.55, 0.9) : vec3(0.7, 0.8, 0.95);

    vec2 p0 = vec2(mainX, 1.1);
    vec2 p1 = vec2(mainX + (rand(vec2(seed + 400.0)) - 0.5) * 0.3, 0.85);
    vec2 p2 = vec2(p1.x + (rand(vec2(seed + 500.0)) - 0.5) * 0.25, 0.6);
    vec2 p3 = vec2(p2.x + (rand(vec2(seed + 600.0)) - 0.5) * 0.2, 0.4);
    vec2 p4 = vec2(p3.x + (rand(vec2(seed + 700.0)) - 0.5) * 0.15, 1.1 - heightVar);

    float s0 = getLightningValue(uv, p0, p1, baseWidth);
    float s1 = getLightningValue(uv, p1, p2, baseWidth * 0.9);
    float s2 = getLightningValue(uv, p2, p3, baseWidth * 0.8);
    float s3 = getLightningValue(uv, p3, p4, baseWidth * 0.7);

    float mainLightning = max(max(s0, s1), max(s2, s3));
    col += coreCol * mainLightning * totalFlash * 2.0;

    if (rand(vec2(seed + 800.0)) > 0.3) {
        float bx1 = p1.x + (rand(vec2(seed + 900.0)) - 0.5) * 0.35;
        vec2 bp0 = vec2(bx1, p1.y);
        vec2 bp1 = vec2(bx1 + (rand(vec2(seed + 1000.0)) - 0.5) * 0.3, bp0.y - rand(vec2(seed + 1100.0)) * 0.45);
        vec2 bp2 = vec2(bp1.x + (rand(vec2(seed + 1200.0)) - 0.5) * 0.2, bp1.y - rand(vec2(seed + 1300.0)) * 0.35);
        float bs0 = getLightningValue(uv, bp0, bp1, baseWidth * 0.5);
        float bs1 = getLightningValue(uv, bp1, bp2, baseWidth * 0.35);
        col += coreCol * max(bs0, bs1) * totalFlash * 0.8;

        if (rand(vec2(seed + 1400.0)) > 0.5) {
            float bx1a = bp1.x + (rand(vec2(seed + 1500.0)) - 0.5) * 0.2;
            vec2 bp0a = vec2(bx1a, bp1.y);
            vec2 bp1a = vec2(bx1a + (rand(vec2(seed + 1600.0)) - 0.5) * 0.15, bp0a.y - rand(vec2(seed + 1700.0)) * 0.3);
            float bsa = getLightningValue(uv, bp0a, bp1a, baseWidth * 0.25);
            col += coreCol * bsa * totalFlash * 0.5;
        }
    }

    if (rand(vec2(seed + 1800.0)) > 0.4) {
        float bx2 = p2.x + (rand(vec2(seed + 1900.0)) - 0.5) * 0.25;
        vec2 bp0 = vec2(bx2, p2.y);
        vec2 bp1 = vec2(bx2 + (rand(vec2(seed + 2000.0)) - 0.5) * 0.25, bp0.y - rand(vec2(seed + 2100.0)) * 0.4);
        float bs = getLightningValue(uv, bp0, bp1, baseWidth * 0.45);
        col += coreCol * bs * totalFlash * 0.7;
    }

    if (rand(vec2(seed + 2200.0)) > 0.45) {
        float bx3 = p3.x + (rand(vec2(seed + 2300.0)) - 0.5) * 0.2;
        vec2 bp0 = vec2(bx3, p3.y);
        vec2 bp1 = vec2(bx3 + (rand(vec2(seed + 2400.0)) - 0.5) * 0.15, bp0.y - rand(vec2(seed + 2500.0)) * 0.3);
        float bs = getLightningValue(uv, bp0, bp1, baseWidth * 0.4);
        col += coreCol * bs * totalFlash * 0.6;
    }

    if (rand(vec2(seed + 2600.0)) > 0.5) {
        float bx4 = p0.x + (rand(vec2(seed + 2700.0)) - 0.5) * 0.2;
        vec2 bp0 = vec2(bx4, p0.y - 0.05);
        vec2 bp1 = vec2(bx4 + (rand(vec2(seed + 2800.0)) - 0.5) * 0.25, bp0.y - rand(vec2(seed + 2900.0)) * 0.35);
        float bs = getLightningValue(uv, bp0, bp1, baseWidth * 0.5);
        col += coreCol * bs * totalFlash * 0.75;
    }

    float distToLightning = min(min(sdSegment(uv, p0, p1), sdSegment(uv, p1, p2)),
                                min(sdSegment(uv, p2, p3), sdSegment(uv, p3, p4)));
    float ambientGlow = smoothstep(baseWidth * 15.0, 0.0, distToLightning);
    col += glowCol * ambientGlow * totalFlash * 0.4;

    float flicker = 0.8 + rand(vec2(u_time * 10000.0, uv.x)) * 0.2;
    col *= flicker;

    col = clamp(col, 0.0, 1.0);
    alpha = max(max(col.r, col.g), col.b);
    alpha = clamp(alpha, 0.0, 1.0);

    outColor = vec4(col, alpha);
}

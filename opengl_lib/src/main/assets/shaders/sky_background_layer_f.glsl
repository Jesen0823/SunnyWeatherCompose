#version 300 es
precision highp float;
layout (location = 0) out vec4 outColor;
uniform float u_time;
uniform vec2 u_screenSize;
uniform int u_timeOfDay;
uniform vec3 u_skyColorTop;
uniform vec3 u_skyColorBottom;
uniform float u_sunIntensity;
uniform float u_moonIntensity;
uniform bool u_sunVisible;
uniform int u_skyMode;

void main() {
    vec2 p = gl_FragCoord.xy / u_screenSize.xy;
    vec3 skyColor = mix(u_skyColorBottom, u_skyColorTop, p.y);
    vec3 result = skyColor;
    
    if (u_skyMode == 1) {
        result = skyColor * 0.85;
    } else if (u_skyMode == 2) {
        result = skyColor * 0.95 + vec3(0.02, 0.02, 0.05);
    } else if (u_skyMode == 3) {
        result = skyColor + vec3(0.05, 0.03, 0.0);
    }
    
    if (u_timeOfDay == 0 && u_sunVisible) {
        vec2 sunPos = vec2(0.70, 0.80);
        float dx = p.x - sunPos.x;
        float dy = p.y - sunPos.y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq > 0.25) {
            outColor = vec4(result, 1.0);
            return;
        }
        
        float dist = sqrt(distSq);
        float angle = atan(dy, dx);

        float sunCore = exp(-dist * 80.0);
        float sunInnerGlow = exp(-dist * 14.0);
        float sunOuterGlow = exp(-dist * 6.0);

        vec3 sunCoreColor = vec3(1.0, 1.0, 0.98);
        vec3 sunInnerColor = vec3(1.0, 0.98, 0.92);
        vec3 sunOuterColor = vec3(1.0, 0.92, 0.75);

        float coreBlend = smoothstep(0.0, 0.025, dist);
        result = mix(sunCoreColor, result, coreBlend);

        result = result + sunInnerColor * sunInnerGlow * 0.5;
        result = result + sunOuterColor * sunOuterGlow * 0.35;

        float rayPattern = sin(angle * 6.0 + u_time * 0.2) * 0.3 + 0.7;
        rayPattern *= sin(angle * 3.0) * 0.2 + 0.8;
        float sunRays = exp(-dist * 4.0);
        vec3 sunRayColor = vec3(1.0, 0.95, 0.85);
        result = result + sunRayColor * sunRays * rayPattern * 0.12;
    } else if (u_timeOfDay == 1) {
        vec2 moonPos = vec2(0.25, 0.8);
        float dx = p.x - moonPos.x;
        float dy = p.y - moonPos.y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq > 0.64) {
            outColor = vec4(result, 1.0);
            return;
        }
        
        float moonDist = sqrt(distSq);
        
        float moonCore = smoothstep(0.065, 0.048, moonDist);
        float moonGlow1 = smoothstep(0.15, 0.065, moonDist);
        float moonGlow2 = smoothstep(0.30, 0.15, moonDist);
        float moonGlow3 = smoothstep(0.50, 0.30, moonDist);
        float moonGlow4 = smoothstep(0.80, 0.50, moonDist);
        
        float moonSurface = 1.0;
        if (moonDist < 0.08) {
            vec2 moonUV = vec2(dx, dy) * 20.0;
            float moonCrater1 = smoothstep(0.18, 0.14, length(moonUV - vec2(0.35, 0.25)));
            float moonCrater2 = smoothstep(0.12, 0.09, length(moonUV + vec2(0.25, -0.35)));
            float moonCrater3 = smoothstep(0.09, 0.06, length(moonUV - vec2(-0.35, 0.15)));
            float moonCrater4 = smoothstep(0.14, 0.10, length(moonUV + vec2(0.15, 0.35)));
            float moonCrater5 = smoothstep(0.07, 0.04, length(moonUV - vec2(0.0, -0.2)));
            float moonCrater6 = smoothstep(0.10, 0.07, length(moonUV + vec2(-0.2, 0.3)));
            
            moonSurface = 1.0 - (moonCrater1 * 0.15 + moonCrater2 * 0.12 + moonCrater3 * 0.10 + moonCrater4 * 0.13 + moonCrater5 * 0.08 + moonCrater6 * 0.11);
            float moonShadow = smoothstep(-0.15, 0.15, moonUV.x) * 0.15;
            moonSurface *= (1.0 - moonShadow);
            
            float moonEdgeSoftness = smoothstep(0.065, 0.055, moonDist) * 0.4;
            moonSurface = mix(moonSurface, moonSurface * 0.65, moonEdgeSoftness);
        }
        
        vec3 moonCoreColor = vec3(1.0, 1.0, 0.98) * u_moonIntensity;
        vec3 moonGlowColor1 = vec3(0.95, 0.96, 0.99) * u_moonIntensity;
        vec3 moonGlowColor2 = vec3(0.88, 0.90, 0.96) * u_moonIntensity * 0.85;
        vec3 moonGlowColor3 = vec3(0.78, 0.80, 0.90) * u_moonIntensity * 0.65;
        vec3 moonGlowColor4 = vec3(0.65, 0.68, 0.80) * u_moonIntensity * 0.45;
        
        float moonCoreGlow = smoothstep(0.08, 0.065, moonDist);
        result = mix(result, moonCoreColor * moonSurface, moonCore);
        result = result + moonGlowColor1 * moonCoreGlow * 0.35;
        result = result + moonGlowColor1 * moonGlow1 * 0.35;
        result = result + moonGlowColor2 * moonGlow2 * 0.25;
        result = result + moonGlowColor3 * moonGlow3 * 0.12;
        result = result + moonGlowColor4 * moonGlow4 * 0.06;
    }
    outColor = vec4(result, 1.0);
}

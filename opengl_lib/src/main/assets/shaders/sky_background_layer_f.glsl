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
        float dist = distance(p, sunPos);
        float angle = atan(p.y - sunPos.y, p.x - sunPos.x);

        float sunCore = exp(-dist * 80.0);
        float sunInnerGlow = exp(-dist * 12.0);
        float sunOuterGlow = exp(-dist * 4.0);

        vec3 sunCoreColor = vec3(1.0, 1.0, 0.98);
        vec3 sunInnerColor = vec3(1.0, 0.98, 0.92);
        vec3 sunOuterColor = vec3(1.0, 0.92, 0.75);

        float coreBlend = smoothstep(0.0, 0.025, dist);
        result = mix(sunCoreColor, result, coreBlend);

        result = result + sunInnerColor * sunInnerGlow * 0.5;
        result = result + sunOuterColor * sunOuterGlow * 0.35;

        float rayPattern = sin(angle * 6.0 + u_time * 0.2) * 0.3 + 0.7;
        rayPattern *= sin(angle * 3.0) * 0.2 + 0.8;
        float sunRays = exp(-dist * 2.5);
        vec3 sunRayColor = vec3(1.0, 0.95, 0.85);
        result = result + sunRayColor * sunRays * rayPattern * 0.12;
    } else if (u_timeOfDay == 1) {
        vec2 moonPos = vec2(0.25, 0.8);
        float moonDist = distance(p, moonPos);
        float moonGlow = 1.0 - smoothstep(0.0, 0.50, moonDist);
        float moonCore = 1.0 - smoothstep(0.0, 0.06, moonDist);
        vec3 moonColor = vec3(0.95, 0.96, 1.0) * u_moonIntensity;
        result = result + moonColor * moonGlow * 0.22;
        result = result + vec3(1.0, 1.0, 1.0) * moonCore * 0.6;
    }
    outColor = vec4(result, 1.0);
}

#version 300 es
precision highp float;
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_speed;
layout(location = 2) in float a_size;
layout(location = 3) in float a_swingOffset;
layout(location = 4) in float a_swingSpeed;
layout(location = 5) in float a_windOffset;
uniform float u_time;
uniform vec2 u_screenSize;
uniform float u_snowSpeed;
uniform float u_windForce;
uniform float u_gustWindX;
uniform float u_gustWindY;
uniform float u_gustStrength;
uniform float u_gustGroupSeed;
uniform float u_gustGroupRatio;
uniform float u_gustLocalTime;
uniform float u_gustBlendFactor;
uniform float u_gustDuration;
float randomHash(float x, float seed) {
    return fract(sin(x * 12.9898 + seed * 78.233) * 43758.5453);
}
void main() {
    vec2 pos = a_position;
    float speedFactor = 0.5 + u_snowSpeed * 2.5;
    pos.y += u_time * a_speed.y * speedFactor * 0.25;
    float windBase = u_windForce * u_time * 0.18;
    float windSwing = u_windForce * sin(u_time * 0.5 + a_swingOffset) * 0.08;
    float randomPerturbX = sin(u_time * a_swingSpeed + a_swingOffset) * 0.02;
    float randomPerturbY = cos(u_time * a_swingSpeed * 1.2 + a_swingOffset) * 0.01;
    float particleHash = randomHash(a_swingOffset * 100.0, u_gustGroupSeed);
    float threshold = 1.0 - u_gustGroupRatio;
    float softWidth = 0.15;
    float gustParticipate = smoothstep(threshold - softWidth, threshold + softWidth, particleHash);
    float particleVariation = randomHash(a_swingOffset * 50.0, u_gustGroupSeed * 2.0);
    float individualStrength = gustParticipate * (0.5 + particleVariation * 1.0);
    float effectiveStrength = u_gustStrength * individualStrength;
    float timeProgress = u_gustLocalTime / u_gustDuration;
    float bellEnvelope = sin(timeProgress * 3.14159);
    float turbulence = sin(u_gustLocalTime * 3.0 + a_swingOffset * 10.0) * 0.5 + 0.5;
    float gustEffectX = u_gustWindX * effectiveStrength * bellEnvelope * turbulence * 0.12;
    float gustEffectY = u_gustWindY * effectiveStrength * bellEnvelope * turbulence * 0.08;
    pos.x += windBase + windSwing + randomPerturbX + gustEffectX;
    pos.y += randomPerturbY + gustEffectY;
    pos.x += a_windOffset * u_windForce * 0.10;
    pos.y = fract(pos.y + 1.25);
    pos.x = fract(pos.x + 0.5);
    vec2 clipPos = pos * 2.0 - 1.0;
    clipPos.x *= u_screenSize.x / u_screenSize.y;
    gl_Position = vec4(clipPos, 0.0, 1.0);
    gl_PointSize = a_size * u_snowSpeed * 32.0;
}

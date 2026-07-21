#version 300 es
layout(location = 0) in float a_tData;
uniform vec4 u_StartEndPoints;
uniform vec4 u_ControlPoints;
uniform mat4 u_MVPMatrix;
uniform float u_Offset;
vec2 bezier3(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3, in float t){
    float tt = (1.0 - t) * (1.0 -t);
    return tt * (1.0 -t) *p0 + 3.0 * t * tt * p1 + 3.0 * t *t *(1.0 -t) *p2
           + t *t *t *p3;
}
vec2 bezier3_(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3, in float t){
    vec2 q0 = mix(p0, p1, t);
    vec2 q1 = mix(p1, p2, t);
    vec2 q2 = mix(p2, p3, t);
    vec2 r0 = mix(q0, q1, t);
    vec2 r1 = mix(q1, q2, t);
    return mix(r0, r1, t);
}
void main() {
    vec4 pos;
    pos.w = 1.0;
    vec2 p0 = u_StartEndPoints.xy;
    vec2 p3 = u_StartEndPoints.zw;
    vec2 p1 = u_ControlPoints.xy;
    vec2 p2 = u_ControlPoints.zw;
    p0.y *= u_Offset;
    p1.y *= u_Offset;
    p2.y *= u_Offset;
    p3.y *= u_Offset;
    float t = a_tData;
    vec2 point = bezier3_(p0, p1, p2, p3, t);
    if (t < 0.0){
        pos.xy = vec2(0.0, 0.0);
    }else{
        pos.xy = point;
    }
    gl_PointSize = 4.0;
    gl_Position = u_MVPMatrix * pos;
}

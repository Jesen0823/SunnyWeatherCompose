#version 300 es
precision highp float;
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in float a_size;
layout(location = 3) in float a_twinkleOffset;
out vec3 v_color;
out float v_twinkleOffset;
uniform vec2 u_screenSize;
void main() {
    vec2 clipPos = a_position * 2.0 - 1.0;
    clipPos.x *= u_screenSize.x / u_screenSize.y;
    gl_Position = vec4(clipPos, 0.0, 1.0);
    gl_PointSize = a_size;
    v_color = a_color;
    v_twinkleOffset = a_twinkleOffset;
}

#version 300 es
precision mediump float;
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 uModel;
uniform mat4 uProjection;
out vec2 vTexCoord;

void main() {
    gl_Position = uProjection * uModel * aPosition;
    vTexCoord = aTexCoord;
}

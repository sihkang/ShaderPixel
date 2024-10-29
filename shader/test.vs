#version 410 core

layout(location = 0) in vec3 apos;

uniform mat4 mvp;

void main() {
    // gl_Position = mvp * vec4(apos, 1.0);
   gl_Position = vec4(apos, 1.0);
}
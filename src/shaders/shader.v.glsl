#version 150

uniform mat4 matrix;

in vec4 position;
in vec3 color;

out vec3 vColor;

void main() {
    gl_Position = matrix * position;
    vColor = color;
}

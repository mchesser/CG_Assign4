#version 150

in vec4 v_coord;
in vec2 texcoord;

out vec2 texCoord;

uniform mat4 rotate;
uniform mat4 proj;


void main(void) {

    gl_Position = proj * rotate * v_coord;

    texCoord = texcoord;

}
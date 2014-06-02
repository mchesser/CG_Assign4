#version 150

in vec4 v_coord;
in vec2 texcoord;

out vec2 texCoord;
out float fog;

uniform mat4 rotate;
uniform mat4 proj;

#define FOG_SCALE 0.05

void main(void) {

    gl_Position = proj * rotate * v_coord;

    texCoord = texcoord;
    fog = v_coord.y * FOG_SCALE;

}
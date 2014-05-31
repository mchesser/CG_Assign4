#version 150

in vec2 texCoord;

out vec4 fColor;

uniform sampler2D day_texture;
uniform sampler2D night_texture;
uniform vec3 sun_position;


void main() {

    fColor = texture2D(day_texture, texCoord);

}
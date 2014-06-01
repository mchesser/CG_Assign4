#version 150

in vec2 texCoord;

out vec4 fColor;

uniform sampler2D baseTexture;


void main() {

    fColor = texture(baseTexture, texCoord);

}

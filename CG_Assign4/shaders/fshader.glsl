#version 120

varying vec4 color;
varying vec2 texcoord;

uniform sampler2D texture;

void main(void)
{
    vec4 texcolor = texture2D(texture, texcoord);
    gl_FragColor = texcolor;
}

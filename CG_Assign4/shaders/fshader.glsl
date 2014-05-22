#version 150

in  vec4 color;
in  vec2 texcoord;
out vec4 fColor;

uniform sampler2D texture;

void main()
{
    vec4 texcolor = texture2D(texture, texcoord);
    fColor = 0.6 * color * texcolor + 0.4 * texcolor;
}

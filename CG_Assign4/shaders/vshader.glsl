#version 120

attribute vec3 v_coord;
attribute vec3 v_normal;
attribute vec2 v_texcoord;

varying vec4 shadowCoord;
varying vec3 normal;
varying vec3 lightDir;
varying vec2 texcoord;

uniform mat4 mv;
uniform mat4 proj;
uniform mat4 depthBiasMVP;
uniform mat3 normalMatrix;

uniform vec3 lightPosition;

void main()
{    
    vec4 pos = mv * vec4(v_coord, 1.0);
    gl_Position = proj * pos;

	shadowCoord = depthBiasMVP * vec4(v_coord, 1.0);
	normal = normalize(normalMatrix * v_normal);
	lightDir = -normalize(lightPosition - vec3(pos));
    texcoord = v_texcoord;
}

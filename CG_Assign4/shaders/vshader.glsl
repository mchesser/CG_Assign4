#version 150

in vec3 v_coord;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 shadowCoord;
out vec3 normal;
out vec3 lightDir;
out vec2 texcoord;
out float depth;

uniform mat4 mv;
uniform mat4 proj;
uniform mat4 depthBiasMVP;
uniform mat3 normalMatrix;

uniform vec3 lightPosition;

void main()
{    
    vec4 pos = mv * vec4(v_coord, 1.0);
    gl_Position = proj * pos;
	depth = pos.z;

	shadowCoord = depthBiasMVP * vec4(v_coord, 1.0);
	normal = normalize(normalMatrix * v_normal);
	lightDir = -normalize(lightPosition - vec3(pos));
    texcoord = v_texcoord;
}

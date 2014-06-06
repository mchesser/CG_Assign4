#version 150

in vec3 v_coord;
in vec3 v_normal;
in vec2 v_texcoord;

out vec4 shadowCoord;
out vec3 normal;
out vec3 sunDir;
out vec2 texcoord;
out vec3 position;

uniform mat4 mv;
uniform mat4 proj;
uniform mat4 depthBiasMVP;
uniform mat3 normalMatrix;
uniform vec3 sunPos;

void main() {
    vec4 pos = mv * vec4(v_coord, 1.0);
    gl_Position = proj * pos;
    position = vec3(pos);

    shadowCoord = depthBiasMVP * vec4(v_coord, 1.0);
    normal = normalize(normalMatrix * v_normal);
    sunDir = -normalize(sunPos - vec3(pos));
    texcoord = v_texcoord;
}

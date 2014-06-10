#version 150

in vec3 v_coord;
in vec3 v_normal;
in vec2 v_texcoord;
in vec3 v_tangent;

out vec4 shadowCoord;
out vec3 normal;
out vec3 sunDir;
out vec2 texcoord;
out vec3 position;
out mat3 localSurface2World;

uniform mat4 m, v;
uniform mat4 proj;
uniform mat4 depthBiasMVP;
uniform mat3 normalMatrix;
uniform vec3 sunPos;

void main() {
    vec4 pos = v * m * vec4(v_coord, 1.0);
    gl_Position = proj * pos;
    position = vec3(pos);

    shadowCoord = depthBiasMVP * vec4(v_coord, 1.0);
    normal = normalize(normalMatrix * v_normal);
    sunDir = -normalize(sunPos - vec3(pos));
    texcoord = v_texcoord;

    // mapping from local surface coordinates to world coordinates
  	localSurface2World[0] = normalize(vec3(m * vec4(v_tangent, 0.0)));
  	localSurface2World[2] = normalize(normalMatrix * v_normal);
  	localSurface2World[1] = normalize(cross(localSurface2World[2], localSurface2World[0]));
}

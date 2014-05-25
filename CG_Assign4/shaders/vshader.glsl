#version 120

attribute vec3 v_coord;
attribute vec3 v_normal;
attribute vec2 v_texcoord;

varying vec4 color;
varying vec2 texcoord;

uniform mat4 mv;
uniform mat4 proj;
uniform mat3 normalMatrix;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
    float opacity;
};
uniform Material material;
uniform vec3 lightPosition;

void main()
{    
    vec4 pos = mv * vec4(v_coord, 1.0);
    gl_Position = proj * pos;

    vec4 ambient, diffuse, specular;
    ambient = vec4(material.ambient, 1.0);

    vec3 L = -normalize(lightPosition - vec3(pos));
    vec3 N = normalize(normalMatrix * v_normal);
    vec3 H = normalize(L - normalize(vec3(pos)));
    
    vec4 Ld = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 Kd = vec4(material.diffuse, 1.0);
    diffuse = Ld * Kd * max(dot(L, N), 0.0);
    
    if (dot(L, N) < 0.0) {
        specular = vec4(0.0, 0.0, 0.0, 1.0); 
    }
    else {
        float Ks = pow(max(dot(N, H), 0.0), material.shine);
        specular = Ks * vec4(material.specular, 1.0);
    }

    color = ambient + diffuse;
    color.a = 1.0;

    texcoord = v_texcoord;
}

#version 150

in vec4 shadowCoord;
in vec3 normal;
in vec2 texcoord;
in vec3 position;

out vec4 out_color;

uniform sampler2D modelTexture;
uniform sampler2DShadow shadowMap;

in vec3 sunDir;
uniform vec3 sunAmbient;
uniform vec3 sunDiffuse;
uniform vec3 sun_position;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
    float opacity;
};
uniform Material material;

vec2 poissonDisk[4] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760)
);

uniform float renderDistance;
float fogFade = 10.0;
vec4 fogColor;

void main(void) {

    
    if (sun_position.y < 0) {
        fogColor = vec4(0.0, 0.0, 0.0, 0.0);
    } else if (sun_position.y < 200) {
        float t = sun_position.y / 200;
        fogColor = (1 - t) * vec4(0.0, 0.0, 0.0, 0.0) + t * vec4(1.0, 1.0, 1.0, 0.0);
    } else {
        fogColor = vec4(1.0, 1.0, 1.0, 0.0);
    }

    // Compute lighting
    vec4 ambient, diffuse;
    ambient = vec4(material.ambient, 1.0);
    
    vec4 Ld = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 Kd = vec4(material.diffuse, 1.0);
    float cosTheta = clamp(dot(normal, sunDir), 0.0, 1.0);
    diffuse = Ld * Kd * cosTheta;

    float bias = 0.005 * tan(acos(cosTheta));
    bias = clamp(bias, 0, 0.01);

    // Compute visibility
    float visibility = 1.0;
    for (int i = 0; i < 4; i++) {
        visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/3000.0, (shadowCoord.z-bias)/shadowCoord.w)));
    }

    vec4 color = ambient * vec4(sunAmbient, 1.0) + visibility * diffuse * vec4(sunDiffuse, 1.0);
    color.a = 1.0;

    vec4 texcolor = texture(modelTexture, texcoord);

	float fogStart = renderDistance - fogFade;
    float fogFactor = min(max(length(position) - fogStart, 0) / (1.0 + fogFade), 1);
    out_color = (1 - fogFactor) * texcolor * color + fogFactor * fogColor; 
}

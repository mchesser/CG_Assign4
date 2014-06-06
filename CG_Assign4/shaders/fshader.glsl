#version 150

in vec4 shadowCoord;
in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec4 out_color;

uniform sampler2D modelTexture;
uniform sampler2DShadow shadowMap;

in vec3 sunDir;
uniform vec3 sunAmbient;
uniform vec3 sunDiffuse;
uniform vec3 sun_position;

uniform bool isDay;

uniform int numLights;
struct LightSource {
    vec3 position;
    vec3 direction;
    float maxAngle;
    vec3 ambient;
    vec3 diffuse;
};
uniform LightSource lights[100];

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
uniform vec4 fogColor;
float fogFade = 10.0;

vec3 computeLighting(LightSource light) {
    vec3 lightToPosition = position - light.position;
    float distance = length(lightToPosition);
    vec3 lightDir = lightToPosition / distance;

    float theta = acos(dot(-lightDir, -light.direction));
    if (theta > light.maxAngle) {
        return vec3(0, 0, 0);
    }

    vec3 ambient = light.ambient * material.ambient;
    float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);

    vec3 diffuse = light.diffuse * material.diffuse * cosTheta * (1.0 - pow(theta / light.maxAngle, 2));

    return (ambient + diffuse) / (1.0 + 0.1 * distance * distance);
}


void main(void) {
    vec4 color;

    if (isDay) {
        // Compute lighting
        vec4 diffuse;

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

        color = visibility * diffuse * vec4(sunDiffuse, 1.0);
        color.a = 1.0;
    }
    else {
        vec3 totalLight = vec3(0.0, 0.0, 0.0);
        for (int i = 0; i < numLights; ++i) {
            totalLight += computeLighting(lights[i]);
        }

        color = vec4(totalLight, 1.0);
        color.a = 1.0;
    }

    color += vec4(material.ambient * sunAmbient, 1.0);

    vec4 texcolor = texture(modelTexture, texcoord);

    float fogStart = renderDistance - fogFade;
    float fogFactor = min(max(length(position) - fogStart, 0) / (1.0 + fogFade), 1);
    out_color = (1 - fogFactor) * texcolor * color + fogFactor * fogColor; 
}

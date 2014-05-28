#version 150

in vec4 shadowCoord;
in vec3 normal;
in vec3 lightDir;
in vec2 texcoord;
in float depth;

out vec4 out_color;

uniform sampler2D modelTexture;
uniform sampler2DShadow shadowMap;

struct Material 
{
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

float fogStart = 40.0;
float fogFade = 10.0;
vec4 fogColor = vec4(0.7, 0.8, 1.0, 1.0);

void main(void)
{
	// Compute lighting
    vec4 ambient, diffuse;
    ambient = vec4(material.ambient, 1.0);
    
    vec4 Ld = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 Kd = vec4(material.diffuse, 1.0);
	float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
    diffuse = Ld * Kd * cosTheta;

	float bias = 0.001 * tan(acos(cosTheta));
	bias = clamp(bias, 0, 0.01);

	// Compute visibility
	float visibility = 1.0;
	for (int i = 0; i < 4; i++) {
		visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/2700.0, (shadowCoord.z-bias)/shadowCoord.w)));
	}

    vec4 color = 0.5 * ambient + visibility * diffuse;
    color.a = 1.0;

    vec4 texcolor = texture(modelTexture, texcoord);
	
	float fogFactor = min(max(-depth - fogStart, 0) / fogFade, 1);
    out_color = (1 - fogFactor) * texcolor * color + fogFactor * fogColor;
}

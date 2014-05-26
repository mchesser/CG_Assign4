#version 120

varying vec4 shadowCoord;
varying vec3 normal;
varying vec3 lightDir;
varying vec2 texcoord;

uniform sampler2D texture;
uniform sampler2D shadowMap;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
    float opacity;
};
uniform Material material;

void main(void)
{
	// Compute lighting
    vec4 ambient, diffuse;
    ambient = vec4(material.ambient, 1.0);
    
    vec4 Ld = vec4(1.0, 1.0, 1.0, 1.0);
    vec4 Kd = vec4(material.diffuse, 1.0);
    diffuse = Ld * Kd * max(dot(normal, lightDir), 0.0);

	float bias = 0.005f;

	// Compute visibility
	float visibility = 1.0;
	if (texture2D(shadowMap, shadowCoord.xy).z < shadowCoord.z - bias) {
		visibility = 0.2;
	}

    vec4 color = ambient + visibility * diffuse;
    color.a = 1.0;

    vec4 texcolor = texture2D(texture, texcoord);
    gl_FragColor = color * texcolor;
}

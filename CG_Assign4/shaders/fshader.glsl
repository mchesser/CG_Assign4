#version 120

varying vec4 shadowCoord;
varying vec3 normal;
varying vec3 lightDir;
varying vec2 texcoord;

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
	
	vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);
	
	for (int i=0;i<4;i++ ){
		visibility -= 0.2*(1.0 - shadow2D(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/3700.0, (shadowCoord.z-bias)/shadowCoord.w)));
	}

	//visibility = (texture(shadowMap, vec3(shadowCoord.xy, (shadowCoord.z-bias)/shadowCoord.w)));

	//if (texture(shadowMap, shadowCoord.xyz, bias) < shadowCoord.z - bias) {
	//	visibility = 0.2;
	//}

    vec4 color = 0.5 * ambient + visibility * diffuse;
    color.a = 1.0;

    vec4 texcolor = texture2D(modelTexture, texcoord);
    gl_FragColor = color * texcolor;
}

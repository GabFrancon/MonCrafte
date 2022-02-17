#version 330 core

struct Material
{
	sampler2DArray textureArray;
	sampler2DArray normalMap;
	sampler2D textureData;
	bool useArrayTex;
	bool useNormalTex;
};

uniform Material material;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

in vec3 fPos;
in vec3 fNor;
in vec2 fTex;
flat in int layer;

out vec4 FragColor;

void main()
{
	vec3 normal;
	if(material.useNormalTex)
	{
		normal = texture(material.normalMap, vec3(fTex, layer)).rgb;
		normal = normalize(normal * 2.0 - 1.0);
	}
	else
		normal = normalize(fNor);
	
	// light calculation
	vec3  lightDir = normalize(lightPos - fPos);
	float ambient  = 0.3;
	float diffuse  = max(dot(normal, lightDir), 0.0);
	vec4  light    = vec4( (ambient + diffuse) * lightColor, 1.0);

	// texture calculation
	vec4 texture;
	if(material.useArrayTex)
		texture = texture(material.textureArray, vec3(fTex, layer));
	else
		texture = texture(material.textureData, fTex);

	FragColor = light * texture;
} 
#version 330 core

struct Material
{
	sampler2DArray textureArray;
	sampler2D textureData;
};

uniform Material material;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform bool useArray;

in vec3 fPos;
in vec3 fNor;
in vec2 fTex;
flat in int layer;

out vec4 FragColor;

void main()
{
	vec3 lightDir = normalize(lightPos - fPos);

	float ambient  = 0.3;
	float diffuse  = max(dot(fNor, lightDir), 0.0);

	vec4 light = vec4( (ambient + diffuse) * lightColor, 1.0);
	vec4 texture;
	if(useArray)
		texture = texture(material.textureArray, vec3(fTex, layer));
	else
		texture = texture(material.textureData, fTex);

	FragColor = light * texture;
} 
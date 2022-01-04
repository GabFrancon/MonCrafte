#version 330 core

struct Material
{
	sampler2D textureData;
};
uniform Material material;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

in vec3 fPos;
in vec3 fNor;
in vec2 fTex;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(fNor);
	vec3 lightDir = normalize(lightPos - fPos);
	vec3 viewDir = normalize(camPos - fPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float ambient  = 0.3;
	float diffuse  = max(dot(norm, lightDir), 0.0);

	vec4 resultLight = vec4( (ambient + diffuse) * lightColor, 1.0);
	vec4 resultTexture = texture(material.textureData, fTex);

	FragColor = resultLight * resultTexture;
} 
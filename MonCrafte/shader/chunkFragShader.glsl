#version 330 core

struct Material
{
	sampler2DArray textureArray;
	sampler2DArray normalMap;
	bool useNormalTex;
};

uniform Material material;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform sampler2D depthMap;

in vec3 fPos;
in vec3 fPosModel;
in vec3 fNor;
in vec2 fTex;
flat in int layer;

out vec4 FragColor;

float shadowCalculation()
{
	vec3 projCoords = fPosModel * 0.5 + 0.5;
	float currentDepth = projCoords.z;

	if(currentDepth > 1.0)
        return 0.0;

	float shadow  = 0.0;
	float bias    = 0.005; 
	float samples = 5.0;
	float offset  = 0.002;

	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			float closestDepth = texture(depthMap, projCoords.xy + vec2(x, y)).r;
			if(currentDepth - bias > closestDepth)
                shadow += 1.0;
		}
	}
	shadow /= (samples * samples);
	return shadow;
}

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

	vec3 lightDir = normalize(lightPos - fPos);
	vec3 viewDir = normalize(camPos - fPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float ambient  = 0.3;
	float diffuse  = max(dot(normal, lightDir), 0.0);
	float specular = 0.3 * pow(max(dot(normal, halfwayDir), 0.0), 32);
	float shadow = shadowCalculation();

	vec4 light = vec4( (ambient + (1.0 - shadow)*(diffuse + specular) ) * lightColor, 1.0);
	vec4 texture = texture(material.textureArray, vec3(fTex, layer));

	FragColor = light * texture;
}

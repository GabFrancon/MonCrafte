#version 330 core


struct Material
{
	sampler2DArray textureArray;
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

	float shadow = 0.0;
	vec2 texelSize = 1.4 / textureSize(depthMap, 0);

	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - 0.005 > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 25.0;
	return shadow;
}

void main()
{
	vec3 lightDir = normalize(lightPos - fPos);
	// vec3 viewDir = normalize(camPos - fPos);
	// vec3 reflectDir = reflect(-lightDir, fNor);

	float ambient  = 0.3;
	float diffuse  = max(dot(fNor, lightDir), 0.0);
	float shadow = shadowCalculation();
	// float specular = 0.3 * pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec4 light = vec4( (ambient + (1.0 - shadow)*diffuse) * lightColor, 1.0);
	vec4 texture = texture(material.textureArray, vec3(fTex, layer));
	FragColor = light * texture;
}

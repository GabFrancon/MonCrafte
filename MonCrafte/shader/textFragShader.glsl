#version 330 core

struct Material
{
	sampler2D textureData;
};
uniform Material material;

in vec2 fTex;
out vec4 FragColor;

void main()
{
    FragColor = texture( material.textureData, fTex );
}
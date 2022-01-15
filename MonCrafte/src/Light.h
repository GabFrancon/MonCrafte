#ifndef LIGHT_H
#define LIGHT_H

#include "Shader.h"

class Light
{
private:
	glm::vec3 position;
	glm::vec3 color;

public:
	Light(glm::vec3 pos, glm::vec3 lightColor)
		: position(pos), color(lightColor) {}

	Light() {}

	void setPosition(glm::vec3 pos) { position = pos; }
	void bindLight(Shader worldShader, Shader playerShader)
	{
		worldShader.use();
		worldShader.setVec3("lightPos", position);
		worldShader.setVec3("lightColor", color);

		playerShader.use();
		playerShader.setVec3("lightPos", position);
		playerShader.setVec3("lightColor", color);
	}
};

typedef std::shared_ptr<Light> LightPtr;

#endif // !LIGHT_H




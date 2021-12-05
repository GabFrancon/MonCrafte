#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 color)
	: lightPos(position), lightColor(color) {}


glm::vec3 Light::getPos()
{
	return lightPos;
}

void Light::setPos(glm::vec3 position)
{
	lightPos = position;
}

void Light::render(const GLuint program, const GLuint texture, std::shared_ptr<Sphere> sun)
{
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor[0], lightColor[1], lightColor[2]);

	sun->render(program, glm::scale(glm::translate(glm::mat4(1.0f), lightPos), glm::vec3(0.2)), texture);
}

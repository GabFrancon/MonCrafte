#ifndef LIGHT_H
#define LIGHT_H

#include "GameObject.h"
#include "Sphere.h"

class Light : public GameObject
{
private:
	glm::vec3 color;

public:
	Light(SpherePtr sphere, glm::vec3 pos, float coeff, GLuint texID, glm::vec3 lightColor, bool transparency=false)
		: GameObject(sphere, pos, coeff, texID, 0, glm::vec3(2.0, 0.0, 0.0), transparency), color(lightColor) {}

	Light() {}

	void render(const GLuint program) override
	{
		glUniform3f(glGetUniformLocation(program, "lightColor"), color.x, color.y, color.z);
		glUniform3f(glGetUniformLocation(program, "lightPos"), position.x, position.y, position.z);
		GameObject::render(program);
	}
};

typedef std::shared_ptr<Light> LightPtr;

#endif // !LIGHT_H




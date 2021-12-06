#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include<memory>
#include "Sphere.h"

#ifndef LIGHT_H
#define LIGHT_H

class Light
{
private:
	glm::vec3 lightPos;
	glm::vec3 lightSize;
	glm::vec3 lightColor;
	std::shared_ptr<Sphere> geometry;
	GLuint texture = 0;
	float ambient = 2;

public:
	Light() {};
	Light(std::shared_ptr<Sphere> geometry, glm::vec3 pos, glm::vec3 color, GLuint texID);

	glm::vec3 getPosition() {return lightPos;}
	void setPosition(glm::vec3 position) {lightPos = position;}
	void setSize(float coeff) { lightSize = glm::vec3(coeff); }

	void render(const GLuint program);
	void freeBuffer();
};

#endif // !LIGHT_H




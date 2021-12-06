
#define _USE_MATH_DEFINES

#include <vector>
#include <memory>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <memory>
#include "Cube.h"

#ifndef BLOCK_H
#define BLOCK_H

class Block
{
private:
	std::shared_ptr<Cube> geometry;
	glm::vec3 position;
	glm::vec3 size;
	GLuint texture = 0;
	float ambient = 0.3;

public:
	Block(std::shared_ptr<Cube> cube, glm::vec3 pos, GLuint texID);

	glm::vec3 getPosition() const {return position;};
	void setPosition(glm::vec3 pos) { position = pos; }
	void setSize(float coeff) { size = glm::vec3(coeff); }
	void setTexture(GLuint tex) { texture = tex; }
	void render(const GLuint program);
	void freeBuffer();
};

#endif // !BLOCK_H


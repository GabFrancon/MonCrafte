#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <vector>
#include<memory>
#include "Block.h"

#ifndef WORLD_H
#define WORLD_H

class World
{
private:
	std::vector<glm::vec3> blockPositions;

public:
	World() {};

	void addBlock(glm::vec3 block);
	void destroyBlock(unsigned int index);
	bool intersect(glm::vec3 camPosition);
	int select(glm::vec3 cam, glm::vec3 camDirection);
	void render(const GLuint program, const GLuint texture, std::shared_ptr<Block> block);
};

#endif // !WORLD_H




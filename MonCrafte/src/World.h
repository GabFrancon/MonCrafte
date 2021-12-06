#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <vector>
#include<memory>
#include <algorithm>

#include "Block.h"

#ifndef WORLD_H
#define WORLD_H

class World
{
private:
	std::vector<glm::vec3> blockPositions;

public:
	World() {};

	void addBlock(glm::vec3 newBlock);
	void addBlock(int blockID, int faceID);
	void destroyBlock(unsigned int index);

	bool intersect(glm::vec3 camPosition);
	std::vector<int> select(glm::vec3 camPos, glm::vec3 lookAt);
	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);

	void render(const GLuint program, const GLuint texture, std::shared_ptr<Block> block);
};

#endif // !WORLD_H




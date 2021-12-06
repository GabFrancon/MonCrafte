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
#include "Light.h"

#ifndef WORLD_H
#define WORLD_H

class World
{
private:
	// ground
	std::vector<std::shared_ptr<Block>> blocks;

	// 3D geometry
	std::shared_ptr<Cube> cube;
	std::shared_ptr<Sphere> sphere;

public:
	World() : cube(std::make_shared<Cube>()), sphere(std::make_shared<Sphere>()) {};

	std::shared_ptr<Cube> getCubeGeometry() { return cube; }
	std::shared_ptr<Sphere> getSphereGeometry() { return sphere; }


	void addBlock(glm::vec3 position, GLuint texture);
	void addBlock(int blockID, int faceID, GLuint texture);
	void destroyBlock(unsigned int index);

	bool intersect(glm::vec3 camPosition);
	std::vector<int> select(glm::vec3 camPos, glm::vec3 lookAt);
	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);

	void bindToGPU();
	void render(const GLuint program);
	void clearBuffers();
};

#endif // !WORLD_H




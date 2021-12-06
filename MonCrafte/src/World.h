#ifndef WORLD_H
#define WORLD_H

#include "Block.h"
#include "Light.h"

class World
{
private:
	// ground
	std::vector<BlockPtr> blocks;
	std::vector<std::vector<std::vector<BlockPtr>>> ground;
	CubePtr cube;
	// light
	std::vector<LightPtr> lights;
	SpherePtr sphere;

public:
	World() : cube(std::make_shared<Cube>()), sphere(std::make_shared<Sphere>()) {};

	CubePtr getCubeGeometry() { return cube; }
	SpherePtr getSphereGeometry() { return sphere; }

	void addBlock(glm::vec3 position, float size, GLuint texture);
	void addBlock(int blockID, int faceID, float size, GLuint texture);
	void destroyBlock(unsigned int index);

	void addLight(glm::vec3 position, float size, GLuint texture, glm::vec3 color);
	void destroyLight(unsigned int index);

	bool intersect(glm::vec3 camPosition);
	std::vector<int> select(glm::vec3 camPos, glm::vec3 lookAt);
	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);

	void genWorld(std::map<std::string, GLuint> textures);
	void bindToGPU();
	void render(const GLuint program);
	void clearBuffers();
};

#endif // !WORLD_H




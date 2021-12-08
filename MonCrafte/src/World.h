#ifndef WORLD_H
#define WORLD_H

#include "Block.h"
#include "Light.h"
#include "Ground.h"

class World
{
private:
	// ground
	Ground ground;
	CubePtr cube;

	// light
	std::vector<LightPtr> lights;
	SpherePtr sphere;

	// textures
	std::map<std::string, GLuint> textures;

public:
	World() {};
	World(std::map<std::string, GLuint> textureCollection);

	CubePtr getCubeGeometry() { return cube; }
	SpherePtr getSphereGeometry() { return sphere; }
	GLuint getTexture(std::string name) { return textures[name]; }

	void addBlock(glm::vec3 position, std::string texName);
	void addBlock(std::string texName);
	void destroyBlock();

	void addLight(glm::vec3 position, float size, GLuint texture, glm::vec3 color);
	void destroyLight(unsigned int index);

	bool collide(glm::vec3 camPosition);
	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);
	
	void genWorld();
	void bindToGPU();
	void render(const GLuint program, glm::vec3 camPos);
	void clearBuffers();

};

#endif // !WORLD_H




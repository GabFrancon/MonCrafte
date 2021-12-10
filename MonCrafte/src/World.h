#ifndef WORLD_H
#define WORLD_H

#include "Chunk.h"
#include "Light.h"
#include "Skybox.h"

class World
{
private:
	// ground
	Chunk chunk;
	CubePtr cube;

	// light
	std::vector<LightPtr> lights;

	// textures
	std::map<std::string, GLuint> textures;

	// skybox
	Skybox skybox;

public:
	World() {};
	World(std::map<std::string, GLuint> textureCollection, GLuint skyTexture);

	CubePtr getCubeGeometry() { return cube; }
	GLuint getTexture(std::string name) { return textures[name]; }

	void addBlock(std::string texName);
	void destroyBlock();

	void addLight(glm::vec3 position, glm::vec3 color);
	void destroyLight(unsigned int index);

	bool collide(glm::vec3 camPosition);
	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);
	
	void genWorld();
	void bindToGPU();
	void bindLights(Shader groundShader, Shader playerShader);
	void render(Shader groundShader,Shader skyShader, glm::vec3 camPos);
	void clearBuffers();

};

#endif // !WORLD_H




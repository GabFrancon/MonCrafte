#ifndef WORLD_H
#define WORLD_H

#include "Chunk.h"
#include "Light.h"
#include "Skybox.h"

class World
{
private:
	// chunks
	int xLimit, yLimit, zLimit;
	int worldSize = 3;                    // in terms of number of chunks
	glm::ivec3 chunkSize = glm::ivec3(15, 25, 15); // in terms of number of blocks
	glm::ivec2 currentChunk = glm::ivec2(0, 0);
	std::vector < std::vector < ChunkPtr >> chunkMap;
	CubePtr cube;

	// lights
	std::vector<LightPtr> lights;

	// textures
	std::map<std::string, Texture> textures;

	// skybox
	Skybox skybox;

public:
	World() {};
	World(std::map<std::string, Texture> textureCollection, GLuint skyTexture);

	CubePtr getCubeGeometry() { return cube; }
	Texture getTexture(std::string name) { return textures[name]; }
	void updateCurrentChunk(glm::vec3 camPos);
	glm::ivec2 toMapCoordinates(glm::ivec2 position);
	glm::ivec2 toWorldCoordinates(glm::ivec2 position);

	void addBlock(std::string texName);
	void destroyBlock();

	void addLight(glm::vec3 position, glm::vec3 color);
	void destroyLight(unsigned int index);

	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);
	
	void genWorld();
	void bindToGPU();
	void bindLights(Shader groundShader, Shader playerShader);
	void render(Shader groundShader,Shader skyShader, glm::vec3 camPos);
	void clearBuffers();


	bool collide(glm::vec3 cam);
	glm::ivec3 toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	bool isInWorld(glm::vec3 objectPos);
	glm::ivec2 cToChunkCoord(glm::ivec2 chunkCoord);
	glm::ivec2 cToWorldCoord(glm::ivec2 chunkCoord);
	glm::ivec2 getAssociatedChunk(int x, int z);
};

#endif // !WORLD_H




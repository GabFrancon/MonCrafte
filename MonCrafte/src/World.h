#ifndef WORLD_H
#define WORLD_H

#include "Chunk.h"
#include "Light.h"
#include "Skybox.h"
#include "PerlinNoise.h"

struct Selection {
	BlockPtr object;
	bool isSelection = false;
	unsigned int faceID = -1;
	float distance = 10.0;
};

class World
{
private:
	// chunks
	int xLimit, yLimit, zLimit;
	int worldSize = 3;                    // in terms of number of chunks
	glm::ivec3 chunkSize = glm::ivec3(15, 25, 15); // in terms of number of blocks
	std::vector < std::vector < ChunkPtr >> chunkMap;
	Selection selection;
	CubePtr cube;

	// lights
	std::vector<LightPtr> lights;

	// textures
	std::map<std::string, Texture> textures;

	// skybox
	Skybox skybox;

public:
	World() : xLimit(0), yLimit(0), zLimit(0) {};
	World(std::map<std::string, Texture> textureCollection, GLuint skyTexture);

	CubePtr getCubeGeometry() { return cube; }
	Texture getTexture(std::string name) { return textures[name]; }

	// coordinates
	glm::ivec3 toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	glm::vec3 toWorldCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	bool isInWorld(glm::vec3 objectPos);

	// blocks
	void addBlock(std::string texName);
	BlockPtr getBlock(glm::vec3 blockPos, glm::ivec2 associatedChunk);
	glm::ivec2 getAssociatedChunk(int x, int z);
	std::map<std::string, BlockPtr> getNeighbours(BlockPtr block);
	void hideNeighboursFace(BlockPtr block);
	void showNeighboursFace(BlockPtr block);
	void destroyBlock();

	// lights
	void addLight(glm::vec3 position, glm::vec3 color);
	void destroyLight(unsigned int index);

	// collision and selection
	bool collide(glm::vec3 cam);
	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);
	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);
	void selectObject(Selection newSelection);

	// generation and rendering
	void genWorld();
	void bindToGPU();
	void bindLights(Shader groundShader, Shader playerShader);
	void render(Shader groundShader, Shader skyShader, glm::vec3 camPos);
	void clearBuffers();
};

#endif // !WORLD_H




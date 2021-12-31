#ifndef WORLD_H
#define WORLD_H

#include "Chunk.h"
#include "Light.h"
#include "Skybox.h"
#include "PerlinNoise.h"

struct Selection {
	BlockPtr object;
	bool     isSelection = false;
	int      faceID      = 0;
	float    distance    = 10.0;
};

class World
{
private:
	// chunks
	int worldSize = 11;								// in terms of number of chunks
	int renderRadius = 4;							// in terms of number of chunks
	glm::ivec3 chunkSize = glm::ivec3(15, 25, 15);  // in terms of number of blocks

	int xLimit     = 0;
	int yLimit     = 0;
	int zLimit     = 0;
	int chunkLimit = 0;

	std::vector<ChunkPtr> chunkMap;
	Selection selection;
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

	// coordinates
	glm::ivec3 toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	glm::vec3  toWorldCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	bool       isInWorld(glm::vec3 objectPos);
	int        chunkIndex(int i, int j);

	// blocks
	void     addBlock(std::string texName);
	BlockPtr getBlock(glm::vec3 blockPos);
	void     destroyBlock();
	void     hideNeighboursFace(BlockPtr block);
	void     showNeighboursFace(BlockPtr block);
	std::map<std::string, BlockPtr> getNeighbours(BlockPtr block);

	// lights
	void addLight(glm::vec3 position, glm::vec3 color);
	void destroyLight(unsigned int index);

	// collision and selection
	bool  collide(glm::vec3 cam);
	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);
	void  updateSelection(glm::vec3 camPos, glm::vec3 lookAt);

	// generation and rendering
	void genWorld();
	void bindToGPU();
	void bindLights(Shader groundShader, Shader playerShader);
	void render(Shader groundShader, Shader skyShader, glm::vec3 camPos, glm::vec3 lookAt);
	void clearBuffers();
};

#endif // !WORLD_H




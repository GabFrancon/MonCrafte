#ifndef WORLD_H
#define WORLD_H

#include "Chunk.h"
#include "Light.h"
#include "Skybox.h"
#include "BiomeHelper.h"
#include "Shader.h"


struct Selection {
	BlockPtr object;
	bool     isSelection = false;
	int      faceID      = 0;
	float    distance    = 6.0;
};

class World
{
private:
	// chunks
	int worldSize = 15;								 // in terms of number of chunks
	int renderRadius = 10;							 // in terms of number of chunks
	glm::ivec3 chunkSize = glm::ivec3(15, 60, 15);   // in terms of number of blocks

	int xLimit     = 0;
	int yLimit     = 0;
	int zLimit     = 0;
	int chunkLimit = 0;

	std::vector<ChunkPtr> chunkMap;
	BiomeHelper biomeHelper;
	Selection selection;

	// lights
	std::vector<LightPtr> lights;

	// textures
	GLuint texArray = 0;

	// skybox
	Skybox skybox;

public:
	World() {};
	World(std::map<std::string, Texture> textureCollection, GLuint textureArray, GLuint skyTexture);

	Texture getTexture(std::string name) { return biomeHelper.getTexture(name); }

	// coordinates
	glm::ivec3 toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	glm::vec3  toWorldCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	glm::ivec2 getRelatedChunk(glm::vec3 blockPos);
	bool       isInWorld(glm::vec3 objectPos);
	int        chunkIndex(int i, int j);

	// blocks
	void     addBlock(std::string texName);
	BlockPtr getBlock(glm::vec3 blockPos);
	void     destroyBlock();
	void     markChunkForRegen(BlockPtr block);
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
	bool isSelection() { return selection.isSelection; }
	glm::vec3 getSelection() { return selection.object->getPosition(); }

	// generation and rendering
	void genWorld();
	void addTree(int x, int y, int z);
	void bindLights(Shader groundShader, Shader playerShader);
	void render(Shader groundShader, Shader skyShader, glm::vec3 camPos, glm::vec3 lookAt);
	void clearBuffers();
};

#endif // !WORLD_H




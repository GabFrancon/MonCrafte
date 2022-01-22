#ifndef WORLD_H
#define WORLD_H

#include "utils.h"
#include "Chunk.h"
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
	int worldSize = 15;								 // in terms of number of chunks
	int renderRadius = 10;							 // in terms of number of chunks
	glm::ivec3 chunkSize = glm::ivec3(15, 60, 15);   // in terms of number of blocks

	int xLimit     = 0;
	int yLimit     = 0;
	int zLimit     = 0;
	int chunkLimit = 0;

	// CPU data
	BiomeHelper biomeHelper;
	Selection selection;

	// geometry
	std::vector<ChunkPtr> chunkMap;
	Skybox skybox;

public:
	World() {};
	World(std::map<std::string, Texture> textureCollection);

	Texture getTexture(std::string name) { return biomeHelper.getTexture(name); }

	// coordinates
	glm::ivec3 toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	glm::vec3  toWorldCoord(glm::ivec3 pos, glm::ivec2 chunkPos);
	glm::ivec2 getRelatedChunk(glm::vec3 blockPos);
	bool       isInWorld(glm::vec3 objectPos);
	int        chunkIndex(int i, int j);

	// blocks
	void     addBlock(Texture tex);
	BlockPtr getBlock(glm::vec3 blockPos);
	void     destroyBlock();
	void     markChunkForRegen(BlockPtr block);
	void     hideNeighboursFace(BlockPtr block);
	void     showNeighboursFace(BlockPtr block);
	std::map<std::string, BlockPtr> getNeighbours(BlockPtr block);

	// collision and selection
	bool	  collide(glm::vec3 cam);
	bool	  collideOnY(glm::vec3 cam);
	float	  faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);
	void	  updateSelection(glm::vec3 playerPos, glm::vec3 lookAt);
	bool      isSelection() { return selection.isSelection; }
	glm::vec3 getSelection() { return selection.object->getPosition(); }

	// generation and rendering
	void genWorld();
	void addTree(int x, int y, int z);
	void simpleRender();
	void render(Shader groundShader, Shader skyShader, glm::vec3 playerPos);
	void clearBuffers();
};

#endif // !WORLD_H

#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

class Chunk
{
public:
	Chunk(glm::ivec3 size);

	void setBlock(glm::ivec3 blockCoords, BlockPtr block);
	BlockPtr getBlock(glm::ivec3 blockCoords);
	int blockIndex(int x, int y, int z);
	bool mustBeRegen() { return regenRequired; }
	bool containsTransparentBlocks() { return hasTransparency; }

	void generateChunk();
	void markForRegen() { regenRequired = true; }
	void render(Shader shader, GLuint texArray);
	void clearBuffers();

private:
	glm::ivec3 chunkSize; // in terms of number of blocks
	std::vector<BlockPtr> blockMap;
	bool hasTransparency = false;
	bool regenRequired = false;

	GLuint vao      = 0;
	GLuint posVbo   = 0;
	GLuint normVbo  = 0;
	GLuint texVbo   = 0;
	GLuint layerVbo = 0;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<float> layers;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

#endif // !CHUNK_H

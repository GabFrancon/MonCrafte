#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

class Chunk
{
public:
	Chunk(glm::ivec3 size) : chunkSize(size) {
		map = std::vector<std::vector<std::vector<BlockPtr>>>
			(size.x, std::vector<std::vector<BlockPtr>>
				(size.y, std::vector<BlockPtr>
					(size.z, nullptr)));
	}


	// objects
	void setBlock(glm::ivec3 blockCoords, BlockPtr block) {
		map[blockCoords.x][blockCoords.y][blockCoords.z] = block;
	}

	BlockPtr getBlock(glm::ivec3 blockCoords) {
		return map[blockCoords.x][blockCoords.y][blockCoords.z];
	}

	// rendering
	void clearBuffers()
	{
		for (int x = 0; x < chunkSize.x; x++)
		{
			for (int y = 0; y < chunkSize.y; y++)
			{
				for (int z = 0; z < chunkSize.z; z++)
				{
					map[x][y][z]->freeBuffer();
				}
			}
		}
	}

private:
	glm::ivec3 chunkSize; // in terms of number of blocks
	std::vector<std::vector<std::vector<BlockPtr>>> map;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

#endif // !CHUNK_H

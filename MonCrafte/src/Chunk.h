#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

class Chunk
{
public:
	Chunk() {
		map = std::vector<std::vector<std::vector<BlockPtr>>>
			(chunkSize.x, std::vector<std::vector<BlockPtr>>
				(chunkSize.y, std::vector<BlockPtr>
					(chunkSize.z, nullptr)));
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
	glm::ivec3 chunkSize = glm::ivec3(15, 25, 15); // in terms of number of blocks
	std::vector<std::vector<std::vector<BlockPtr>>> map;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

#endif // !CHUNK_H

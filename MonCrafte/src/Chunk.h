#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

class Chunk
{
public:
	Chunk(glm::ivec3 size) : chunkSize(size)
	{
		blockMap = std::vector<BlockPtr>(size.x * size.y * size.z, nullptr);
	}

	void setBlock(glm::ivec3 blockCoords, BlockPtr block)
	{
		blockMap[blockIndex(blockCoords.x, blockCoords.y, blockCoords.z)] = block;
	}

	BlockPtr getBlock(glm::ivec3 blockCoords)
	{
		return blockMap[blockIndex(blockCoords.x, blockCoords.y, blockCoords.z)];
	}

	int blockIndex(int x, int y, int z)
	{
		return x + y * chunkSize.x + z * chunkSize.x * chunkSize.y;
	}

	void clearBuffers()
	{
		for (int x = 0; x < chunkSize.x; x++)
		{
			for (int y = 0; y < chunkSize.y; y++)
			{
				for (int z = 0; z < chunkSize.z; z++)
				{
					blockMap[blockIndex(x, y, z)]->freeBuffer();
				}
			}
		}
	}

private:
	glm::ivec3 chunkSize; // in terms of number of blocks
	std::vector<BlockPtr> blockMap;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

#endif // !CHUNK_H

#include "Chunk.h"

Chunk::Chunk(glm::ivec3 size) : chunkSize(size)
{
	blockMap = std::vector<BlockPtr>(size.x * size.y * size.z, nullptr);
	glGenVertexArrays(1, &vao);
}

void Chunk::setBlock(glm::ivec3 blockCoords, BlockPtr block)
{
	blockMap[blockIndex(blockCoords.x, blockCoords.y, blockCoords.z)] = block;
}

BlockPtr Chunk::getBlock(glm::ivec3 blockCoords)
{
	return blockMap[blockIndex(blockCoords.x, blockCoords.y, blockCoords.z)];
}

int Chunk::blockIndex(int x, int y, int z)
{
	return x + y * chunkSize.x + z * chunkSize.x * chunkSize.y;
}

void Chunk::generateChunk()
{
	// first clear CPU data
	positions.clear();
	normals.clear();
	texCoords.clear();
	texIndices.clear();
	hasTransparency = false;
	std::vector<BlockPtr> transparentBlocks;

	// then add solid blocks (CPU)
	for (BlockPtr block : blockMap)
	{
		if (block->isTransparent() && !block->isHidden())
		{
			hasTransparency = true;
			transparentBlocks.push_back(block);
		}

		else if (block->isSolid() && !block->isHidden())
			block->addGeometry(positions, normals, texCoords, texIndices);
	}
	// then transparent ones (CPU)
	for (BlockPtr block : transparentBlocks)
			block->addGeometry(positions, normals, texCoords, texIndices);

	// finally clear buffers before bind GPU buffers again
	clearVBOs();
	bindVBOs();
	regenRequired = false;
}

void Chunk::bindVBOs()
{
	glBindVertexArray(vao);

	// position coordinates
	size_t vertexBufferSize = sizeof(float) * positions.size();
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, positions.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal coordinates
	size_t normalBufferSize = sizeof(float) * normals.size();
	glGenBuffers(1, &normVbo);
	glBindBuffer(GL_ARRAY_BUFFER, normVbo);
	glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normals.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// texture coordinates
	size_t textureBufferSize = sizeof(float) * texCoords.size();
	glGenBuffers(1, &texVbo);
	glBindBuffer(GL_ARRAY_BUFFER, texVbo);
	glBufferData(GL_ARRAY_BUFFER, textureBufferSize, texCoords.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	// texture indices in texArray
	size_t layerBufferSize = sizeof(float) * texIndices.size();
	glGenBuffers(1, &indVbo);
	glBindBuffer(GL_ARRAY_BUFFER, indVbo);
	glBufferData(GL_ARRAY_BUFFER, layerBufferSize, texIndices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void Chunk::render()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, positions.size());
	glBindVertexArray(0);
}

void Chunk::clearVBOs()
{
	glDeleteBuffers(1, &posVbo);
	glDeleteBuffers(1, &normVbo);
	glDeleteBuffers(1, &texVbo);
	glDeleteBuffers(1, &indVbo);
}
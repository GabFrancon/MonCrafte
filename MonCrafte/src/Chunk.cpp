#include "Chunk.h"

Chunk::Chunk(glm::ivec3 size) : chunkSize(size)
{
	blockMap = std::vector<BlockPtr>(size.x * size.y * size.z, nullptr);

	// generate the VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// position coordinates
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal coordinates
	glGenBuffers(1, &normVbo);
	glBindBuffer(GL_ARRAY_BUFFER, normVbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// texture coordinates
	glGenBuffers(1, &texVbo);
	glBindBuffer(GL_ARRAY_BUFFER, texVbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	// layer
	glGenBuffers(1, &layerVbo);
	glBindBuffer(GL_ARRAY_BUFFER, layerVbo);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
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
	vertices.clear();
	normals.clear();
	uvs.clear();
	layers.clear();
	hasTransparency = false;

	// first add solid blocks
	for (BlockPtr block : blockMap)
	{
		if (block->isTransparent())
			hasTransparency = true;

		if (block->isSolid() && !block->isHidden())
			block->addGeometry(vertices, normals, uvs, layers);
	}
	// then transparent ones
	for (BlockPtr block : blockMap)
	{
		if (block->isTransparent() && !block->isHidden())
			block->addGeometry(vertices, normals, uvs, layers);
	}

	glBindVertexArray(vao);

	// position coordinates
	size_t vertexBufferSize = sizeof(float) * vertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices.data(), GL_DYNAMIC_READ);

	// normals coordinates
	size_t normalBufferSize = sizeof(float) * normals.size();
	glBindBuffer(GL_ARRAY_BUFFER, normVbo);
	glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normals.data(), GL_DYNAMIC_READ);

	// texture coordinates
	size_t textureBufferSize = sizeof(float) * uvs.size();
	glBindBuffer(GL_ARRAY_BUFFER, texVbo);
	glBufferData(GL_ARRAY_BUFFER, textureBufferSize, uvs.data(), GL_DYNAMIC_READ);

	// layer
	size_t layerBufferSize = sizeof(float) * layers.size();
	glBindBuffer(GL_ARRAY_BUFFER, layerVbo);
	glBufferData(GL_ARRAY_BUFFER, layerBufferSize, layers.data(), GL_DYNAMIC_READ);

	glBindVertexArray(0);
	regenRequired = false;
}

void Chunk::render(Shader shader, GLuint texArray)
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Chunk::clearBuffers()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &posVbo);
	glDeleteBuffers(1, &normVbo);
	glDeleteBuffers(1, &texVbo);
	glDeleteBuffers(1, &layerVbo);
}
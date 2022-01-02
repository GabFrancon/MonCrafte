#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

class Chunk
{
public:
	Chunk(glm::ivec3 size) : chunkSize(size)
	{
		blockMap = std::vector<BlockPtr>(size.x * size.y * size.z, nullptr);

		// generate the VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// position coordinates
		size_t vertexBufferSize = sizeof(float) * vertices.size();
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices.data(), GL_DYNAMIC_READ);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// normal coordinates
		size_t normalBufferSize = sizeof(float) * normals.size();
		glGenBuffers(1, &nbo);
		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normals.data(), GL_DYNAMIC_READ);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);

		// texture coordinates
		size_t textureBufferSize = sizeof(float) * uvs.size();
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ARRAY_BUFFER, ebo);
		glBufferData(GL_ARRAY_BUFFER, textureBufferSize, uvs.data(), GL_DYNAMIC_READ);		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(2);

		// layer
		size_t layerBufferSize = sizeof(float) * types.size();
		glGenBuffers(1, &lbo);
		glBindBuffer(GL_ARRAY_BUFFER, lbo);
		glBufferData(GL_ARRAY_BUFFER, layerBufferSize, types.data(), GL_DYNAMIC_READ);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);
	}

	void setBlock(glm::ivec3 blockCoords, BlockPtr block)
	{
		blockMap[blockIndex(blockCoords.x, blockCoords.y, blockCoords.z)] = block;
	}

	BlockPtr getBlock(glm::ivec3 blockCoords)
	{
		return blockMap[blockIndex(blockCoords.x, blockCoords.y, blockCoords.z)];
	}

	bool isTransparent() { return hasTransparency; }

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

	void generateChunk()
	{
		vertices.clear();
		hasTransparency = false;

		for(int x = 0 ; x < chunkSize.x ; x++)
			for (int y = 0; y < chunkSize.y; y++)
				for (int z = 0; z < chunkSize.z; z++)
				{
					BlockPtr block = getBlock(glm::ivec3(x, y, z));
					if (!block->isEmpty() && !block->isHidden())
					{
						if (block->isTransparent())
							hasTransparency = true;

						glm::vec3 position = block->getPosition();
						float xoffset = position.x;
						float yoffset = position.y;
						float zoffset = position.z;

						if (block->isFaceVisible("right"))
						{
							vertices.insert(vertices.end(),
								{
									 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset
								});
							normals.insert(normals.end(),
								{
									 1.0f, 0.0f, 0.0f,
									 1.0f, 0.0f, 0.0f,
									 1.0f, 0.0f, 0.0f,
									 1.0f, 0.0f, 0.0f,
									 1.0f, 0.0f, 0.0f,
									 1.0f, 0.0f, 0.0f
								});
							uvs.insert(uvs.end(),
								{
									 1.0f, 0.0f,
									 0.0f, 1.0f,
									 0.0f, 0.0f,
									 1.0f, 0.0f,
									 1.0f, 1.0f,
									 0.0f, 1.0f
								});
							types.insert(types.end(),
								{
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray()
								});
						}
						if (block->isFaceVisible("left"))
						{
							vertices.insert(vertices.end(),
								{
									-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset
								});
							normals.insert(normals.end(),
								{
									-1.0f, 0.0f, 0.0f,
									-1.0f, 0.0f, 0.0f,
									-1.0f, 0.0f, 0.0f,
									-1.0f, 0.0f, 0.0f,
									-1.0f, 0.0f, 0.0f,
									-1.0f, 0.0f, 0.0f
								});
							uvs.insert(uvs.end(),
								{
									 0.0f, 1.0f,
									 1.0f, 1.0f,
									 1.0f, 0.0f,
									 0.0f, 1.0f,
									 1.0f, 0.0f,
									 0.0f, 0.0f
								});
							types.insert(types.end(),
								{
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray()
								});
						}
						if (block->isFaceVisible("top"))
						{
							vertices.insert(vertices.end(),
								{
									 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset
								});
							normals.insert(normals.end(),
								{
									 0.0f, 1.0f, 0.0f,
									 0.0f, 1.0f, 0.0f,
									 0.0f, 1.0f, 0.0f,
									 0.0f, 1.0f, 0.0f,
									 0.0f, 1.0f, 0.0f,
									 0.0f, 1.0f, 0.0f
								});
							uvs.insert(uvs.end(),
								{
									 1.0f, 1.0f,
									 1.0f, 0.0f,
									 0.0f, 0.0f,
									 1.0f, 1.0f,
									 0.0f, 0.0f,
									 0.0f, 1.0f
								});
							types.insert(types.end(),
								{
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray()
								});
						}
						if (block->isFaceVisible("bottom"))
						{
							vertices.insert(vertices.end(),
								{
									 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset
								});
							normals.insert(normals.end(),
								{
									 0.0f,-1.0f, 0.0f,
									 0.0f,-1.0f, 0.0f,
									 0.0f,-1.0f, 0.0f,
									 0.0f,-1.0f, 0.0f,
									 0.0f,-1.0f, 0.0f,
									 0.0f,-1.0f, 0.0f
								});
							uvs.insert(uvs.end(),
								{
									 1.0f, 1.0f,
									 0.0f, 0.0f,
									 1.0f, 0.0f,
									 1.0f, 1.0f,
									 0.0f, 1.0f,
									 0.0f, 0.0f
								});
							types.insert(types.end(),
								{
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray()
								});
						}
						if (block->isFaceVisible("front"))
						{
							vertices.insert(vertices.end(),
								{
									-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset, 0.5f + zoffset,
									 0.5f + xoffset, 0.5f + yoffset, 0.5f + zoffset
								});
							normals.insert(normals.end(),
								{
									 0.0f, 0.0f, 1.0f,
									 0.0f, 0.0f, 1.0f,
									 0.0f, 0.0f, 1.0f,
									 0.0f, 0.0f, 1.0f,
									 0.0f, 0.0f, 1.0f,
									 0.0f, 0.0f, 1.0f
								});
							uvs.insert(uvs.end(),
								{
									 0.0f, 0.0f,
									 0.0f, 1.0f,
									 1.0f, 1.0f,
									 0.0f, 0.0f,
									 1.0f, 1.0f,
									 1.0f, 0.0f
								});
							types.insert(types.end(),
								{
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray()
								});
						}
						if (block->isFaceVisible("back"))
						{
							vertices.insert(vertices.end(),
								{
									 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset, 0.5f + yoffset,-0.5f + zoffset,
									 0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset,
									-0.5f + xoffset,-0.5f + yoffset,-0.5f + zoffset
								});
							normals.insert(normals.end(),
								{
									 0.0f, 0.0f,-1.0f,
									 0.0f, 0.0f,-1.0f,
									 0.0f, 0.0f,-1.0f,
									 0.0f, 0.0f,-1.0f,
									 0.0f, 0.0f,-1.0f,
									 0.0f, 0.0f,-1.0f
								});
							uvs.insert(uvs.end(),
								{
									 1.0f, 0.0f ,
									 0.0f, 1.0f,
									 0.0f, 0.0f,
									 1.0f, 0.0f,
									 1.0f, 1.0f,
									 0.0f, 1.0f
								});
							types.insert(types.end(),
								{
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray(),
									(float)block->getTexture().getLocationInArray()
								});
						}
					}
				}
		glBindVertexArray(vao);

		// position coordinates
		size_t vertexBufferSize = sizeof(float) * vertices.size();
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices.data(), GL_DYNAMIC_READ);

		// normals coordinates
		size_t normalBufferSize = sizeof(float) * normals.size();
		glBindBuffer(GL_ARRAY_BUFFER, nbo);
		glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normals.data(), GL_DYNAMIC_READ);

		// texture coordinates
		size_t textureBufferSize = sizeof(float) * uvs.size();
		glBindBuffer(GL_ARRAY_BUFFER, ebo);
		glBufferData(GL_ARRAY_BUFFER, textureBufferSize, uvs.data(), GL_DYNAMIC_READ);

		// layer
		size_t layerBufferSize = sizeof(float) * types.size();
		glBindBuffer(GL_ARRAY_BUFFER, lbo);
		glBufferData(GL_ARRAY_BUFFER, layerBufferSize, types.data(), GL_DYNAMIC_READ);

		glBindVertexArray(0);
	}

	void render(Shader shader, GLuint texArray)
	{
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}

private:
	glm::ivec3 chunkSize; // in terms of number of blocks
	std::vector<BlockPtr> blockMap;
	bool hasTransparency = false;

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint nbo = 0;
	GLuint lbo = 0;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<float> types;
};

typedef std::shared_ptr<Chunk> ChunkPtr;

#endif // !CHUNK_H

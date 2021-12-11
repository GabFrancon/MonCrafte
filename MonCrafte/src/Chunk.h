#ifndef CHUNK_H
#define CHUNK_H

#include "Block.h"

class Chunk
{
public:
	Chunk() {};
	Chunk(glm::ivec2 position, CubePtr geometry, std::map<std::string, Texture> textures);

	// coordinates
	glm::ivec3 toChunkCoord(glm::vec3 pos);
	glm::vec3 toWorldCoord(glm::ivec3 coords);
	bool isInChunk(glm::ivec3 coords);
	// objects
	void addBlock(Texture texture, bool transparency = false);
	void destroyBlock();
	void selectObject(glm::vec3 position);
	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);
	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);
	bool collideObject(BlockPtr object, glm::vec3 position);
	bool collideGround(glm::vec3 cam);
	// rendering
	std::map<std::string, BlockPtr> getNeighbours(BlockPtr block);
	void hideNeighboursFace(BlockPtr block);
	void showNeighboursFace(BlockPtr block);
	void render(Shader shader, glm::vec3 camPos);
	void clearBuffers();

private:
	glm::ivec3 chunkSize = glm::ivec3(15, 25, 15); // in terms of number of blocks
	glm::ivec2 chunkPos; // compared to other chunks

	glm::vec3 selection;
	bool isSelection = false;
	unsigned int selectedFace;
	float distanceToSelection;

	std::vector<std::vector<std::vector<BlockPtr>>> map;
};


#endif // !CHUNK_H
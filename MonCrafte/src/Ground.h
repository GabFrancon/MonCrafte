#ifndef GROUND_H
#define GROUND_H

#include "Block.h"

class Ground
{
public:
	Ground() {};
	Ground(int width, int height, int length, CubePtr cube, std::map<std::string, GLuint> textures);

	// coordinates
	glm::vec3 toWorldCoordinates(int x, int y, int z);
	std::vector<int> toMapCoordinates(glm::vec3 worldCoords);
	bool isInMap(glm::vec3 position);
	// objects
	void addBlock(GLuint texture, bool transparency = false);
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
	void render(GLuint program, glm::vec3 camPos);
	void clearBuffers();

private:
	int halfWidth = 10;
	int halfHeight = 10;
	int halfLength = 10;

	glm::vec3 selection;
	bool isSelection = false;
	unsigned int selectedFace = 6;
	float distanceToSelection = 0.0f;

	std::vector<std::vector<std::vector<BlockPtr>>> map;
};


#endif // !GROUND_H
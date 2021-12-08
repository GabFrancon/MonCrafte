#ifndef GROUND_H
#define GROUND_H

#include "Block.h"

class Ground
{
public:
	Ground() {};
	Ground(int width, int height, int length, CubePtr cube, std::map<std::string, GLuint> textures);

	void addBlock(glm::vec3 position, GLuint texture, bool transparency = false);
	void addBlock(GLuint texture, bool transparency = false);
	void destroyBlock();

	void selectObject(glm::vec3 position);
	bool collideObject(BlockPtr object, glm::vec3 position);
	bool collideGround(glm::vec3 cam);
	void setNeighbourRender(BlockPtr block);

	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);
	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);

	void render(GLuint program, glm::vec3 camPos);
	void clearBuffers();

	bool isInMap(glm::vec3 position);
	bool isOnBorders(glm::vec3 position);
	std::vector<BlockPtr> getNeighbours(BlockPtr block);
	glm::vec3 toWorldCoordinates(int x, int y, int z);
	std::vector<int> toMapCoordinates(glm::vec3 worldCoords);


private:
	int halfWidth;
	int halfHeight;
	int halfLength;

	bool isSelection;
	glm::vec3 selection;
	unsigned int selectedFace;
	float distanceToSelection;


	std::vector<std::vector<std::vector<BlockPtr>>> map;
};


#endif // !GROUND_H
#ifndef GROUND_H
#define GROUND_H

#include "Block.h"

class Ground
{
public:
	Ground() {};
	Ground(int width, int height, int length, CubePtr cube);

	void addBlock(glm::vec3 position, GLuint texture, GLuint selectTex, bool transparency = false);
	void addBlock(GLuint texture, GLuint selectTex, bool transparency = false);
	void destroyBlock();

	void selectObject(glm::vec3 position);
	bool intersect(glm::vec3 cam);

	float faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal);
	void updateSelection(glm::vec3 camPos, glm::vec3 lookAt);

	void genGround(std::map<std::string, GLuint> textures);
	void render(GLuint program, glm::vec3 camPos);
	void clearBuffers();

	bool isInMap(glm::vec3 position);
	glm::vec3 toWorldCoordinates(std::vector<int> mapCoords);
	std::vector<int> toMapCoordinates(glm::vec3 worldCoords);


private:
	int halfWidth;
	int halfHeight;
	int halfLength;

	CubePtr cube;
	BlockPtr emptyBlock;

	bool isSelection;
	glm::vec3 selection;
	unsigned int selectedFace;
	float distanceToSelection;


	std::vector<std::vector<std::vector<BlockPtr>>> map;
};


#endif // !GROUND_H
#ifndef BLOCK_H
#define BLOCK_H

#include "GameObject.h"
#include "Cube.h"

class Block : public GameObject
{
public:
	Block(CubePtr cube, glm::vec3 pos, float coeff, GLuint texID, GLuint selTexID, bool transparency)
		: GameObject(cube, pos, coeff, texID, selTexID, glm::vec3(0.3, 1.0, 0.5), transparency, true) {}

	Block(CubePtr cube, glm::vec3 pos, GLuint selTexID)
		: GameObject(cube, pos, 1.0f, 0, selTexID, glm::vec3(0.3, 1.0, 0.5), true, false) {}
};

typedef std::shared_ptr<Block> BlockPtr;

#endif // !BLOCK_H


#include "World.h"

void World::addBlock(glm::vec3 block)
{
	blockPositions.push_back(block);
}
void World::destroyBlock(unsigned int index)
{
	blockPositions.erase(blockPositions.begin() + index);
}
bool World::intersect(glm::vec3 cam)
{
	for (int i = 0; i < blockPositions.size(); i++)
	{
		glm::vec3 block = blockPositions[i];

		bool xAxis = (cam.x - 0.5 < block.x + 0.5) && (cam.x + 0.5 > block.x - 0.5);
		bool yAxis = (cam.y - 0.5 < block.y + 0.5) && (cam.y + 0.5 > block.y - 0.5);
		bool zAxis = (cam.z - 0.5 < block.z + 0.5) && (cam.z + 0.5 > block.z - 0.5);

		if (xAxis && yAxis && zAxis)
			return true;
	}
	return false;
}

int World::select(glm::vec3 camPos, glm::vec3 lookAt)
{
	int nearestCube = -1;
	float smallestDist = 8;

	for (int i = 0; i < blockPositions.size(); i++)
	{
		glm::vec3 block = blockPositions[i];

		// front
		glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0);
		glm::vec3 point  = block + normal/2;

		if (glm::dot(lookAt, normal) != 0)
		{
			float iDist = glm::dot(point - camPos, normal) / glm::dot(lookAt, normal);

			if ((0 < iDist) && (iDist < smallestDist))
			{
				glm::vec3 iPoint = camPos + iDist * lookAt;
				bool xAxis = (point.x-0.5 < iPoint.x) && (iPoint.x < point.x+0.5);
				bool yAxis = (point.y-0.5 < iPoint.y) && (iPoint.y < point.y+0.5);
				if (xAxis && yAxis)
				{
					smallestDist = iDist;
					nearestCube = i;
				}
			}
		}
	}
	if (nearestCube >= 0)
	{
		glm::vec3 selected = blockPositions[nearestCube];
		std::cout << "selected : x=" << selected.x << ", y=" << selected.y << ", z=" << selected.z << "\n" << std::endl;
	}
	else
		std::cout << "no cube selected\n" << std::endl;

	return nearestCube;
}

void World::render(const GLuint program, const GLuint texture, std::shared_ptr<Block> block)
{
	for (unsigned int i = 0; i < blockPositions.size(); i++)
		block->render(program, glm::translate(glm::mat4(1.0f), blockPositions[i]), texture);
}
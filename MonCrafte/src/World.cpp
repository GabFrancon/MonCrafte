#include "World.h"

void World::addBlock(glm::vec3 position, GLuint texture)
{
	blocks.push_back(std::make_shared<Block>(cube, position, texture));
}

void World::addBlock(int blockID, int faceID, GLuint texture)
{
	glm::vec3  position = blocks[blockID]->getPosition();
	switch (faceID)
	{
	case 0:
		position += glm::vec3(0.0, 0.0, 1.0);
		break;
	case 1:
		position -= glm::vec3(0.0, 0.0, 1.0);
		break;
	case 2:
		position += glm::vec3(0.0, 1.0, 0.0);
		break;
	case 3:
		position -= glm::vec3(0.0, 1.0, 0.0);
		break;
	case 4:
		position += glm::vec3(1.0, 0.0, 0.0);
		break;
	case 5:
		position -= glm::vec3(1.0, 0.0, 0.0);
		break;
	default:
		break;
	}
	addBlock(position, texture);
}

void World::destroyBlock(unsigned int index)
{
	blocks.erase(blocks.begin() + index);
}

bool World::intersect(glm::vec3 cam)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		glm::vec3 block = blocks[i]->getPosition();

		bool xAxis = (cam.x - 0.5 < block.x + 0.5) && (cam.x + 0.5 > block.x - 0.5);
		bool yAxis = (cam.y - 0.5 < block.y + 0.5) && (cam.y + 0.5 > block.y - 0.5);
		bool zAxis = (cam.z - 0.5 < block.z + 0.5) && (cam.z + 0.5 > block.z - 0.5);

		if (xAxis && yAxis && zAxis)
			return true;
	}
	return false;
}

float World::faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal)
{
	float dist = 10;
	if (glm::dot(lookAt, normal) != 0)
	{
		float iDist = glm::dot(point - camPos, normal) / glm::dot(lookAt, normal);

		if ((0 < iDist) && (iDist < 8))
		{
			bool abscissa = false;
			bool ordinate = false;
			glm::vec3 iPoint = camPos + iDist * lookAt;

			if (normal.x != 0)
			{
				abscissa = (point.y - 0.5 < iPoint.y) && (iPoint.y < point.y + 0.5);
				ordinate = (point.z - 0.5 < iPoint.z) && (iPoint.z < point.z + 0.5);
			}
			else if (normal.y != 0)
			{
				abscissa = (point.x - 0.5 < iPoint.x) && (iPoint.x < point.x + 0.5);
				ordinate = (point.z - 0.5 < iPoint.z) && (iPoint.z < point.z + 0.5);
			}
			else if (normal.z != 0)
			{
				abscissa = (point.x - 0.5 < iPoint.x) && (iPoint.x < point.x + 0.5);
				ordinate = (point.y - 0.5 < iPoint.y) && (iPoint.y < point.y + 0.5);
			}

			if (abscissa && ordinate)
				dist = iDist;	
		}
	}
	return dist;
}

std::vector<int> World::select(glm::vec3 camPos, glm::vec3 lookAt)
{
	int nearestCube = -1;
	int nearestFace = -1;
	float minDist = 10;

	for (int i = 0; i < blocks.size(); i++)
	{
		glm::vec3 block = blocks[i]->getPosition();

		float frontDist  = faceDistance(camPos, lookAt, block + glm::vec3(0.0, 0.0, 0.5), glm::vec3( 0.0, 0.0, 1.0));
		float backDist   = faceDistance(camPos, lookAt, block - glm::vec3(0.0, 0.0, 0.5), glm::vec3( 0.0, 0.0,-1.0));
		float topDist    = faceDistance(camPos, lookAt, block + glm::vec3(0.0, 0.5, 0.0), glm::vec3( 0.0, 1.0, 0.0));
		float bottomDist = faceDistance(camPos, lookAt, block - glm::vec3(0.0, 0.5, 0.0), glm::vec3( 0.0,-1.0, 0.0));
		float rightDist  = faceDistance(camPos, lookAt, block + glm::vec3(0.5, 0.0, 0.0), glm::vec3( 1.0, 0.0, 0.0));
		float leftDist   = faceDistance(camPos, lookAt, block - glm::vec3(0.5, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0));

		std::vector<float> dist{frontDist, backDist, topDist, bottomDist, rightDist, leftDist};
		auto minD = std::min_element(dist.begin(), dist.end());

		if (*minD < minDist)
		{
			minDist = *minD;
			nearestCube = i;
			nearestFace = std::distance(dist.begin(), minD);
		}
	}

	if (nearestCube >= 0)
	{
		glm::vec3 selected = blocks[nearestCube]->getPosition();
		std::cout
			<< "x=" << selected.x
			<< ", y=" << selected.y
			<< ", z=" << selected.z
			<< " - face " << nearestFace
			<< " - dist " << minDist
			<< "\n" << std::endl;
	}
	else
		std::cout << "no cube selected\n" << std::endl;

	int faceDist = std::round(minDist);
	std::vector<int> selection = { nearestCube, nearestFace, faceDist };
	return selection;
}

void World::bindToGPU()
{
	cube->bindToGPU();
	sphere->bindToGPU();
}

void World::render(const GLuint program)
{
	for (unsigned int i = 0; i < blocks.size(); i++)
		blocks[i]->render(program);
}

void World::clearBuffers()
{
	for (int i = 0 ; i < blocks.size() ; i++)
		blocks[i]->freeBuffer();
}
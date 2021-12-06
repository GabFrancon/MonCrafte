#include "World.h"

void World::addBlock(glm::vec3 position, float size, GLuint texture)
{
	blocks.push_back(std::make_shared<Block>(cube, position, size, texture));
}

void World::addBlock(int blockID, int faceID, float size, GLuint texture)
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
	addBlock(position, size, texture);
}

void World::destroyBlock(unsigned int index)
{
	blocks.erase(blocks.begin() + index);
}

void World::addLight(glm::vec3 position, float size, GLuint texture, glm::vec3 color)
{
	lights.push_back(std::make_shared<Light>(sphere, position, size, texture, color));
}

void World::destroyLight(unsigned int index)
{
	lights.erase(lights.begin() + index);
}


bool World::intersect(glm::vec3 cam)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		glm::vec3 block = blocks[i]->getPosition();

		bool xAxis = (cam.x - 0.3 < block.x + 0.3) && (cam.x + 0.3 > block.x - 0.3);
		bool yAxis = (cam.y - 1.0 < block.y + 1.0) && (cam.y + 1.0 > block.y - 1.0);
		bool zAxis = (cam.z - 0.3 < block.z + 0.3) && (cam.z + 0.3 > block.z - 0.3);

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

		if (0 < iDist)
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
	unsigned int count = 0;
	for (int i = 0; i < blocks.size(); i++)
	{
		glm::vec3 block = blocks[i]->getPosition();
		float xDiff = block.x - camPos.x;
		float yDiff = block.y - camPos.y;
		float zDiff = block.z - camPos.z;
		if ( std::sqrt(xDiff * xDiff + yDiff * yDiff + zDiff*zDiff) < 8)
		{
			count++;
			float frontDist = faceDistance(camPos, lookAt, block + glm::vec3(0.0, 0.0, 0.5), glm::vec3(0.0, 0.0, 1.0));
			float backDist = faceDistance(camPos, lookAt, block - glm::vec3(0.0, 0.0, 0.5), glm::vec3(0.0, 0.0, -1.0));
			float topDist = faceDistance(camPos, lookAt, block + glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 1.0, 0.0));
			float bottomDist = faceDistance(camPos, lookAt, block - glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, -1.0, 0.0));
			float rightDist = faceDistance(camPos, lookAt, block + glm::vec3(0.5, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
			float leftDist = faceDistance(camPos, lookAt, block - glm::vec3(0.5, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0));

			std::vector<float> dist{ frontDist, backDist, topDist, bottomDist, rightDist, leftDist };
			auto minD = std::min_element(dist.begin(), dist.end());

			if (*minD < minDist)
			{
				minDist = *minD;
				nearestCube = i;
				nearestFace = std::distance(dist.begin(), minD);
			}
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
			<< std::endl;
	}
	else
		std::cout << "no cube selected" << std::endl;

	std::cout << "cube tested : " << count << "\n" << std::endl;
	int faceDist = std::round(minDist);
	std::vector<int> selection = { nearestCube, nearestFace, faceDist };
	return selection;
}

void World::genWorld(std::map<std::string, GLuint> textures)
{
	addLight(
		glm::vec3(20.0, 20.0, -20.0),                   // position
		1.0,                                            // size
		textures["sun"],							    // texture
		glm::vec3(255.f/255, 255.f/255, 255.f/255));    // color

	for (int i = -25; i < 26; i++)
	{
		for(int j = -25; j < 26 ; j++)
			addBlock(
				glm::vec3((float) i, 0.0f, (float) j),          // position
				1.0,                                            // size
				textures["stone"]);								// texture
	}

	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.0, 15.0);

	for (int k = 0; k < 100 ; k++)
	{
		int x = std::round(distribution(generator));
		int z = std::round(distribution(generator));
		if(std::abs(x)<26 && std::abs(z)<26)
			addBlock(
				glm::vec3((float) x, 1.0f, (float) z),			// position
				1.0,                                            // size
				textures["dirt"]);								// texture
	}
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
	for (int j = 0; j < lights.size(); j++)
		lights[j]->render(program);
}

void World::clearBuffers()
{
	for (int i = 0 ; i < blocks.size() ; i++)
		blocks[i]->freeBuffer();

	for (int j = 0; j < lights.size(); j++)
		lights[j]->freeBuffer();
}
#include "World.h"

World::World(std::map<std::string, Texture> textureCollection, GLuint skyTexture) :
	cube(std::make_shared<Cube>()),
	textures(textureCollection),
	skybox(Skybox(skyTexture))
{
	xLimit = std::floor(chunkSize.x * worldSize / 2);
	yLimit = std::floor(chunkSize.y / 2);
	zLimit = std::floor(chunkSize.z * worldSize / 2);
	chunkMap = std::vector<std::vector<ChunkPtr>>(worldSize, std::vector<ChunkPtr>(worldSize, nullptr));
	bindToGPU();
}

void World::bindToGPU()
{
	cube->initBuffers();
	skybox.initBuffers();
}


BlockPtr World::getBlock(glm::vec3 blockPos, glm::ivec2 associatedChunk)
{
	glm::ivec2 coords = associatedChunk + glm::ivec2(std::floor(worldSize / 2));
	glm::ivec3 posInChunk = toChunkCoord(blockPos, associatedChunk);
	return chunkMap[coords.x][coords.y]->getBlock(posInChunk);
}

void World::addBlock(std::string texName)
{
	if (texName != "None" && selection.isSelection && selection.distance > 1.5f)
	{
		glm::vec3  position = selection.object->getPosition();
		switch (selection.faceID)
		{
		case 0:position += glm::vec3(0.0, 0.0, 1.0); break;
		case 1:position -= glm::vec3(0.0, 0.0, 1.0); break;
		case 2:position += glm::vec3(0.0, 1.0, 0.0); break;
		case 3:position -= glm::vec3(0.0, 1.0, 0.0); break;
		case 4:position += glm::vec3(1.0, 0.0, 0.0); break;
		case 5:position -= glm::vec3(1.0, 0.0, 0.0); break;
		default: break;
		}
		if (isInWorld(position))
		{
			BlockPtr newBlock = getBlock(position, getAssociatedChunk(position.x, position.z));
			newBlock->fillObject(textures[texName], texName.back() == '+');
			hideNeighboursFace(newBlock);
		}	
	}
}

void World::destroyBlock()
{
	if (selection.isSelection)
	{
		selection.object->emptyObject();
		showNeighboursFace(selection.object);
	}
}

void World::addLight(glm::vec3 position, glm::vec3 color)
{
	lights.push_back(std::make_shared<Light>(position, color));
}

void World::destroyLight(unsigned int index)
{
	lights.erase(lights.begin() + index);
}

glm::ivec3 World::toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos)
{
	int x = pos.x - chunkSize.x * chunkPos.x + std::floor((float)chunkSize.x / 2);
	int y = pos.y                            + std::floor((float)chunkSize.y / 2);
	int z = pos.z - chunkSize.z * chunkPos.y + std::floor((float)chunkSize.z / 2);
	return glm::ivec3(x, y, z);
}

glm::vec3 World::toWorldCoord(glm::ivec3 pos, glm::ivec2 chunkPos)
{
	float x = pos.x + chunkSize.x * chunkPos.x - std::floor((float)chunkSize.x / 2);
	float y = pos.y                            - std::floor((float)chunkSize.y / 2);
	float z = pos.z + chunkSize.z * chunkPos.y - std::floor((float)chunkSize.z / 2);
	return glm::vec3(x, y, z);
}

bool World::isInWorld(glm::vec3 objectPos)
{
	if (-xLimit <= objectPos.x && objectPos.x <= xLimit)
		if (-yLimit <= objectPos.y && objectPos.y <= zLimit)
			if (-zLimit <= objectPos.z && objectPos.z <= zLimit)
				return true;
	return false;
}


glm::ivec2 World::getAssociatedChunk(int x, int z)
{
	int coordX = std::floor((x + (float)chunkSize.x / 2) / chunkSize.x);
	int coordZ = std::floor((z + (float)chunkSize.z / 2) / chunkSize.z);
	return glm::ivec2(coordX, coordZ);
}

bool World::collide(glm::vec3 cam)
{
	int minX = std::max((int)std::trunc(cam.x - 1), -xLimit);
	int maxX = std::min((int)std::trunc(cam.x + 1),  xLimit);
	int minZ = std::max((int)std::trunc(cam.z - 1), -zLimit);
	int maxZ = std::min((int)std::trunc(cam.z + 1),  zLimit);
	int minY = std::max((int)std::trunc(cam.y - 1), -yLimit);
	int maxY = std::min((int)std::trunc(cam.y + 1),  yLimit);

	for (int x = minX; x <= maxX ; x++)
		if ((cam.x - 0.4 < x + 0.4) && (cam.x + 0.4 > x - 0.4))
			for (int z = minZ; z <= maxZ; z++)
				if ((cam.z - 0.4 < z + 0.4) && (cam.z + 0.4 > z - 0.4))
				{
					glm::ivec2 associatedChunk = getAssociatedChunk(x, z);

					for (int y = minY; y <= maxY; y++)
						if ((cam.y - 0.4 < y + 0.4) && (cam.y + 0.4 > y - 0.4))
						{
							BlockPtr object = getBlock(glm::vec3(x,y,z), associatedChunk);
							if (!(object->isEmpty() || object->isTransparent() || object->isHidden()))
								return true;
						}
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

void World::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	Selection newSelection;

	int minX = std::max((int)std::trunc((lookAt.x >=0) * camPos.x + (lookAt.x <=0) * (camPos.x - 5)),-xLimit);
	int maxX = std::min((int)std::trunc((lookAt.x <=0) * camPos.x + (lookAt.x >=0) * (camPos.x + 5)), xLimit);
	int minY = std::max((int)std::trunc((lookAt.y >=0) * camPos.y + (lookAt.y <=0) * (camPos.y - 5)),-yLimit);
	int maxY = std::min((int)std::trunc((lookAt.y <=0) * camPos.y + (lookAt.y >=0) * (camPos.y + 5)), yLimit);
	int minZ = std::max((int)std::trunc((lookAt.z >=0) * camPos.z + (lookAt.z <=0) * (camPos.z - 5)),-zLimit);
	int maxZ = std::min((int)std::trunc((lookAt.z <=0) * camPos.z + (lookAt.z >=0) * (camPos.z + 5)), zLimit);

	for (int x = minX; x <= maxX; x++) {
		for (int z = minZ; z <= maxZ; z++)
		{
			glm::ivec2 associatedChunk = getAssociatedChunk(x, z);
			for (int y = minY; y <= maxY; y++)
			{
				glm::vec3 pos = glm::vec3(x, y, z);
				BlockPtr block = getBlock(pos, associatedChunk);
				if(!(block->isEmpty() || block->isHidden()))
				{
					float frontDist  = faceDistance(camPos, lookAt, pos + glm::vec3(0.0, 0.0, 0.5), glm::vec3( 0.0, 0.0, 1.0));
					float backDist   = faceDistance(camPos, lookAt, pos - glm::vec3(0.0, 0.0, 0.5), glm::vec3( 0.0, 0.0,-1.0));
					float topDist    = faceDistance(camPos, lookAt, pos + glm::vec3(0.0, 0.5, 0.0), glm::vec3( 0.0, 1.0, 0.0));
					float bottomDist = faceDistance(camPos, lookAt, pos - glm::vec3(0.0, 0.5, 0.0), glm::vec3( 0.0,-1.0, 0.0));
					float rightDist  = faceDistance(camPos, lookAt, pos + glm::vec3(0.5, 0.0, 0.0), glm::vec3( 1.0, 0.0, 0.0));
					float leftDist   = faceDistance(camPos, lookAt, pos - glm::vec3(0.5, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0));
					std::vector<float> dist{ frontDist, backDist, topDist, bottomDist, rightDist, leftDist };

					auto minD = std::min_element(dist.begin(), dist.end());
					if (*minD < newSelection.distance)
					{
						newSelection.isSelection = true;
						newSelection.object = block;
						newSelection.distance = *minD;
						newSelection.faceID = std::distance(dist.begin(), minD);
					}
				}
			}
		}
	}
	selectObject(newSelection);
}

void World::selectObject(Selection newSelection)
{
	if (selection.isSelection)
		selection.object->setPointed(false);

	if (newSelection.isSelection)
		newSelection.object->setPointed(true);

	selection = newSelection;
}

std::map<std::string, BlockPtr> World::getNeighbours(BlockPtr block)
{
	glm::vec3 position = block->getPosition();
	std::map<std::string, BlockPtr> neighbours;
	glm::vec3 neighborPos;
	glm::ivec2 associatedChunk;

	if (position.x < xLimit - 1)
	{
		neighborPos = position + glm::vec3(1.0, 0.0, 0.0);
		associatedChunk = getAssociatedChunk(neighborPos.x, neighborPos.z);
		neighbours["left"] = getBlock(neighborPos, associatedChunk);
	}
	if (position.x > -xLimit + 1)
	{
		neighborPos = position - glm::vec3(1.0, 0.0, 0.0);
		associatedChunk = getAssociatedChunk(neighborPos.x, neighborPos.z);
		neighbours["right"] = getBlock(neighborPos, associatedChunk);
	}
	if (position.z < zLimit - 1)
	{
		neighborPos = position + glm::vec3(0.0, 0.0, 1.0);
		associatedChunk = getAssociatedChunk(neighborPos.x, neighborPos.z);
		neighbours["back"] = getBlock(neighborPos, associatedChunk);
	}
	if (position.z > -zLimit + 1)
	{
		neighborPos = position - glm::vec3(0.0, 0.0, 1.0);
		associatedChunk = getAssociatedChunk(neighborPos.x, neighborPos.z);
		neighbours["front"] = getBlock(neighborPos, associatedChunk);
	}

	associatedChunk = getAssociatedChunk(position.x, position.z);
	if (position.y < yLimit - 1)
		neighbours["bottom"] = getBlock(position + glm::vec3(0.0, 1.0, 0.0), associatedChunk);

	if (position.y > -yLimit + 1)
		neighbours["top"] = getBlock(position - glm::vec3(0.0, 1.0, 0.0), associatedChunk);

	return neighbours;
}

void World::hideNeighboursFace(BlockPtr block)
{
	if (!block->isEmpty())
	{
		std::map<std::string, BlockPtr> neighbours = getNeighbours(block);

		for (auto& it : neighbours)
		{
			it.second->setFaceRendering(it.first, block->isTransparent() && !it.second->isTransparent() && !it.second->isEmpty());
			block->setFaceRendering(block->getReversedFace(it.first), (!block->isTransparent() && it.second->isTransparent()) || it.second->isEmpty());
		}
	}
}

void World::showNeighboursFace(BlockPtr block)
{
	if (block->isTransparent() || block->isEmpty())
	{
		std::map<std::string, BlockPtr> neighbours = getNeighbours(block);

		for (auto& it : neighbours)
			it.second->setFaceRendering(it.first, !it.second->isEmpty());
	}
}

void World::genWorld()
{
	glm::ivec2 offset = glm::ivec2(std::floor(worldSize / 2));

	for(int i = 0 ; i < worldSize ; i++)
		for (int j = 0; j < worldSize; j++)
		{
			glm::ivec2 chunkPos = glm::ivec2(i, j) - offset;
			chunkMap[i][j] = std::make_shared<Chunk>();

			for (int x = 0; x < chunkSize.x; x++) {
				for (int y = 0; y < chunkSize.y; y++) {
					for (int z = 0; z < chunkSize.z; z++)
					{
						glm::ivec3 coords = glm::ivec3(x, y, z);
						glm::ivec3 blockPos = toWorldCoord(coords, chunkPos);

						if (y < 5)
							chunkMap[i][j]->setBlock(coords, std::make_shared<Block>(Type::SOLID, cube, blockPos, textures["stone"]));
						else
							chunkMap[i][j]->setBlock(coords, std::make_shared<Block>(Type::AIR, cube, blockPos, Texture()));
					}
				}
			}
		}
	for (int i = 0; i < worldSize; i++)
		for (int j = 0; j < worldSize; j++)
			for (int x = 0; x < chunkSize.x; x++)
				for (int y = 0; y < chunkSize.y; y++)
					for (int z = 0; z < chunkSize.z; z++)
						hideNeighboursFace(chunkMap[i][j]->getBlock(glm::ivec3(x, y, z)));

	addLight(
		glm::vec3(20.0, 20.0, -20.0),						  // position
		glm::vec3(255.f / 255, 255.f / 255, 255.f / 255));    // color
}


void World::bindLights(Shader groundShader, Shader playerShader)
{
	for (int j = 0; j < lights.size(); j++)
		lights[j]->bindLight(groundShader, playerShader);
}

void World::render(Shader groundShader, Shader skyShader, glm::vec3 camPos)
{
	// render skybox
	skybox.render(skyShader);

	// render ground
	groundShader.use();
	std::vector<BlockPtr> transparentBlocks;

	for (int i = 0; i < worldSize; i++)
		for (int j = 0; j < worldSize; j++)
			for (int x = 0; x < chunkSize.x; x++)
				for (int y = 0; y < chunkSize.y; y++)
					for (int z = 0; z < chunkSize.z; z++)
					{
						BlockPtr block = chunkMap[i][j]->getBlock(glm::ivec3(x, y, z));
						if (!(block->isEmpty() || block->isHidden()))
						{
							if (block->isTransparent())
								transparentBlocks.push_back(block);
							else
								block->render(groundShader);
						}
					}

	if (transparentBlocks.size() != 0)
	{
		std::map<float, BlockPtr> sorted;
		for (BlockPtr block : transparentBlocks)
		{
			float distance = glm::length(camPos - block->getPosition());
			sorted[distance] = block;
		}
		for (std::map<float, BlockPtr>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
			it->second->render(groundShader);
	}
}

void World::clearBuffers()
{
	for (int x = 0; x < worldSize; x++)
		for (int y = 0; y < worldSize; y++)
			if (chunkMap[x][y])
				chunkMap[x][y]->clearBuffers();
}

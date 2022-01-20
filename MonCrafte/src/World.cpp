#include "World.h"

World::World(std::map<std::string, Texture> textureCollection) :
	biomeHelper(BiomeHelper(textureCollection, worldSize))
{
	xLimit     = std::floor(chunkSize.x * worldSize / 2);
	yLimit     = std::floor(chunkSize.y / 2) - 1;
	zLimit     = std::floor(chunkSize.z * worldSize / 2);
	chunkLimit = std::floor(worldSize / 2);

	chunkMap = std::vector<ChunkPtr>(worldSize * worldSize, nullptr);
	skybox.init();
	skybox.bindVBOs();
}

int World::chunkIndex(int i, int j)
{
	return i + chunkLimit + (j + chunkLimit) * worldSize;
}

glm::ivec3 World::toChunkCoord(glm::ivec3 pos, glm::ivec2 chunkPos)
{
	int x = pos.x - chunkSize.x * chunkPos.x + std::floor((float)chunkSize.x / 2);
	int y = pos.y + std::floor((float)chunkSize.y / 2);
	int z = pos.z - chunkSize.z * chunkPos.y + std::floor((float)chunkSize.z / 2);
	return glm::ivec3(x, y, z);
}

glm::vec3 World::toWorldCoord(glm::ivec3 pos, glm::ivec2 chunkPos)
{
	float x = pos.x + chunkSize.x * chunkPos.x - std::floor((float)chunkSize.x / 2);
	float y = pos.y - std::floor((float)chunkSize.y / 2);
	float z = pos.z + chunkSize.z * chunkPos.y - std::floor((float)chunkSize.z / 2);
	return glm::vec3(x, y, z);
}

glm::ivec2 World::getRelatedChunk(glm::vec3 blockPos)
{
	int chunkX = std::floor((blockPos.x + (float)chunkSize.x / 2) / chunkSize.x);
	int chunkZ = std::floor((blockPos.z + (float)chunkSize.z / 2) / chunkSize.z);
	return glm::ivec2(chunkX, chunkZ);
}


bool World::isInWorld(glm::vec3 objectPos)
{
	if (-xLimit <= objectPos.x && objectPos.x <= xLimit)
		if (-yLimit <= objectPos.y && objectPos.y <= yLimit)
			if (-zLimit <= objectPos.z && objectPos.z <= zLimit)
				return true;
	return false;
}

BlockPtr World::getBlock(glm::vec3 blockPos)
{
	glm::ivec2 chunkPos = getRelatedChunk(blockPos);
	return chunkMap[chunkIndex(chunkPos.x, chunkPos.y)]->getBlock(toChunkCoord(blockPos, glm::ivec2(chunkPos.x, chunkPos.y)));
}


void World::addBlock(Texture tex)
{
	if (tex.getType() != Type::AIR && selection.isSelection && selection.distance > 1.5f)
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
			BlockPtr newBlock = getBlock(position);
			newBlock->fillObject(tex);
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

void World::markChunkForRegen(BlockPtr block)
{
	glm::ivec2 chunkPos = getRelatedChunk(block->getPosition());
	chunkMap[chunkIndex(chunkPos.x, chunkPos.y)]->markForRegen();
}


void World::addLight(LightPtr light)
{
	lights.push_back(light);
}

void World::destroyLight(unsigned int index)
{
	lights.erase(lights.begin() + index);
}

bool World::collide(glm::vec3 cam)
{
	if(!isInWorld(cam))
		return false;

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
					for (int y = minY; y <= maxY; y++)
						if ((cam.y - 0.4 < y + 0.4) && (cam.y + 0.4 > y - 0.4))
						{
							BlockPtr object = getBlock(glm::vec3(x,y,z));
							if (object->isSolid())
								return true;
						}
	return false;
}

float World::faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal)
{
	float dist = 6.f;
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
	selection.isSelection = false;
	selection.distance = 6.f;

	int minX = std::max((int)std::trunc(camPos.x - 5), -xLimit);
	int maxX = std::min((int)std::trunc(camPos.x + 5), xLimit);
	int minY = std::max((int)std::trunc(camPos.y - 5), -yLimit);
	int maxY = std::min((int)std::trunc(camPos.y + 5), yLimit);
	int minZ = std::max((int)std::trunc(camPos.z - 5), -zLimit);
	int maxZ = std::min((int)std::trunc(camPos.z + 5), zLimit);

	for (int x = minX; x <= maxX; x++)
		for (int z = minZ; z <= maxZ; z++)
			for (int y = minY; y <= maxY; y++)
			{
				glm::vec3 pos = glm::vec3(x, y, z);
				BlockPtr block = getBlock(pos);
				if(block->isSolid() && !block->isHidden())
				{
					std::vector<float> dist{
						faceDistance(camPos, lookAt, pos + glm::vec3(0.0, 0.0, 0.5), glm::vec3(0.0, 0.0, 1.0)),
						faceDistance(camPos, lookAt, pos - glm::vec3(0.0, 0.0, 0.5), glm::vec3(0.0, 0.0,-1.0)),
						faceDistance(camPos, lookAt, pos + glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 1.0, 0.0)),
						faceDistance(camPos, lookAt, pos - glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0,-1.0, 0.0)),
						faceDistance(camPos, lookAt, pos + glm::vec3(0.5, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)),
						faceDistance(camPos, lookAt, pos - glm::vec3(0.5, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0))
					};
					auto minD = std::min_element(dist.begin(), dist.end());

					if (*minD < selection.distance)
					{
						selection.isSelection = true;
						selection.object = block;
						selection.distance = *minD;
						selection.faceID = std::distance(dist.begin(), minD);
					}
				}
			}
}

std::map<std::string, BlockPtr> World::getNeighbours(BlockPtr block)
{
	glm::vec3 position = block->getPosition();
	std::map<std::string, BlockPtr> neighbours;

	if (position.x < xLimit)
		neighbours["left"]   = getBlock(position + glm::vec3(1.0, 0.0, 0.0));
	if (position.x > -xLimit)
		neighbours["right"]  = getBlock(position - glm::vec3(1.0, 0.0, 0.0));
	if (position.z < zLimit)
		neighbours["back"]   = getBlock(position + glm::vec3(0.0, 0.0, 1.0));
	if (position.z > -zLimit)
		neighbours["front"]  = getBlock(position - glm::vec3(0.0, 0.0, 1.0));
	if (position.y < yLimit)
		neighbours["bottom"] = getBlock(position + glm::vec3(0.0, 1.0, 0.0));
	if (position.y > -yLimit)
		neighbours["top"]    = getBlock(position - glm::vec3(0.0, 1.0, 0.0));

	return neighbours;
}

void World::hideNeighboursFace(BlockPtr block)
{
	if (!block->isEmpty())
	{
		std::map<std::string, BlockPtr> neighbours = getNeighbours(block);

		for (auto& it : neighbours)
		{
			bool previous = it.second->isFaceVisible(it.first);
			bool next = block->isTransparent() && !it.second->isTransparent() && !it.second->isEmpty();
			if (next != previous)
			{
				it.second->setFaceRendering(it.first, next);
				markChunkForRegen(it.second);
			}
			block->setFaceRendering(block->getReversedFace(it.first), (!block->isTransparent() && it.second->isTransparent()) || it.second->isEmpty());
		}
	}
	glm::vec3 pos = block->getPosition();
	if (pos.x == -xLimit)
		block->setFaceRendering("left", false);
	if (pos.x == xLimit)
		block->setFaceRendering("right", false);
	if (pos.y == -yLimit-1)
		block->setFaceRendering("bottom", false);
	if (pos.z == -zLimit)
		block->setFaceRendering("back", false);
	if (pos.z == zLimit)
		block->setFaceRendering("front", false);

	markChunkForRegen(block);
}

void World::showNeighboursFace(BlockPtr block)
{
	if (block->isEmpty())
	{
		std::map<std::string, BlockPtr> neighbours = getNeighbours(block);

		for (auto& it : neighbours)
		{
			bool previous = it.second->isFaceVisible(it.first);
			bool next = !it.second->isEmpty();
			if (next != previous)
			{
				it.second->setFaceRendering(it.first, next);
				markChunkForRegen(it.second);
			}
		}
		markChunkForRegen(block);
	}
}

void World::genWorld()
{
	// first generate ground (CPU side)

	for (int i = -chunkLimit ; i <= chunkLimit ; i++)
		for (int j = -chunkLimit ; j <= chunkLimit ; j++)
		{
			int index = chunkIndex(i, j);
			chunkMap[index] = std::make_shared<Chunk>(chunkSize);
			glm::ivec2 chunkPos(i, j);
			Biome biome = biomeHelper.getRelatedBiome(chunkPos);

			/*auto it = textures.begin();
			do {
				it = textures.begin();
				std::advance(it, rand() % textures.size());
			} while (it->second.getType() == Type::TRANSPARENT);*/

			for (int x = 0; x < chunkSize.x; x++)
				for (int z = 0; z < chunkSize.z; z++)
					for (int y = 0; y < chunkSize.y; y++)
					{
						glm::ivec3 posInChunk = glm::ivec3(x, y, z);
						glm::ivec3 blockPos = toWorldCoord(posInChunk, chunkPos);
						BlockPtr element = biomeHelper.genElement(blockPos, chunkPos);
						chunkMap[index]->setBlock(posInChunk, element);
					}
		}

	// then add decorations (only trees for the moment, CPU side)
	for (int i = -chunkLimit; i <= chunkLimit; i++)
		for (int j = -chunkLimit; j <= chunkLimit; j++)
		{
			glm::vec2 chunkPos(i, j);
			Biome biome = biomeHelper.getRelatedBiome(chunkPos);
			for(int count = 0; count < biome.treeDensity ; count++)
			{
				int x = rand() % chunkSize.x;
				int z = rand() % chunkSize.z;
				glm::vec3 blockPos = toWorldCoord(glm::ivec3(x, (int)chunkSize.y/2, z), chunkPos);

				if (isInWorld(blockPos))
				{
					if (!getBlock(blockPos)->isTransparent())
					{
						int height = biomeHelper.getHeight(blockPos.x, blockPos.z, chunkPos);
						bool treeCanBeGen = true;

						for (int a = blockPos.x - 3; a <= blockPos.x + 3; a++)
							for (int b = blockPos.z - 3; b <= blockPos.z + 3; b++)
							{
								glm::vec3 neighborPos = glm::vec3(a, height, b);
								if (isInWorld(neighborPos))
								{
									if (!getBlock(neighborPos)->isEmpty())
										treeCanBeGen = false;
								}
								else
									treeCanBeGen = false;
							}
						if (treeCanBeGen)
							addTree(blockPos.x, height, blockPos.z);
					}
				}
			}
		}

	// generate chunk meshes based on the visible faces (GPU side)
	for (ChunkPtr chunk : chunkMap)
	{
		for (int x = 0; x < chunkSize.x; x++)
			for (int y = 0; y < chunkSize.y; y++)
				for (int z = 0; z < chunkSize.z; z++)
					hideNeighboursFace(chunk->getBlock(glm::ivec3(x, y, z)));

		chunk->markForRegen();
	}
}

void World::addTree(int x, int y, int z)
{
	Texture leaves = biomeHelper.getTexture("oakleaves");
	Texture wood = biomeHelper.getTexture("oaklog");

	for (int j = y; j < y + 6; j++)
	{
		if (j > y + 1 && j < y + 4)
		{
			for (int i = x - 2; i < x + 3; i++)
				for (int k = z - 2; k < z + 3; k++)
					if (!(i == x - 2 && k == z - 2) && !(i == x - 2 && k == z + 2) && !(i == x + 2 && k == z - 2) && !(i == x + 2 && k == z + 2))
					{
						BlockPtr block = getBlock(glm::vec3(i, j, k));
						block->fillObject(leaves);
					}
		}
		else if (j == y + 4)
		{
			for (int i = x - 1; i < x + 2; i++)
				for (int k = z - 1; k < z + 2; k++)
				{
					BlockPtr block = getBlock(glm::vec3(i, j, k));
					block->fillObject(leaves);
				}
		}
		else if (j == y + 5)
		{
			for (int i = x - 1; i < x + 2; i++)
				for (int k = z - 1; k < z + 2; k++)
					if (!(i == x - 1 && k == z - 1) && !(i == x - 1 && k == z + 1) && !(i == x + 1 && k == z - 1) && !(i == x + 1 && k == z + 1))
					{
						BlockPtr block = getBlock(glm::vec3(i, j, k));
						block->fillObject(leaves);
					}
		}

		if (j < y + 4)
		{
			BlockPtr block = getBlock(glm::vec3(x, j, z));
			block->fillObject(wood);
		}
	}
}

void World::bindLights(Shader groundShader, Shader playerShader)
{
	for (int j = 0; j < lights.size(); j++)
		lights[j]->bindLight(groundShader, playerShader);
}

void World::renderForShadowMap(Shader shadowMapShader, glm::vec3 camPos)
{
	shadowMapShader.use();
	LightPtr light = lights[0];
	light->setupCameraForShadowMapping(shadowMapShader, glm::vec3(0.f), 100.f, camPos);
	light->bindShadowMap();

	//int minI = std::max((int)std::round(camPos.x / chunkSize.x - renderRadius),-chunkLimit);
	//int maxI = std::min((int)std::round(camPos.x / chunkSize.x + renderRadius), chunkLimit);
	//int minJ = std::max((int)std::round(camPos.z / chunkSize.z - renderRadius),-chunkLimit);
	//int maxJ = std::min((int)std::round(camPos.z / chunkSize.z + renderRadius), chunkLimit);

	for (int i = -chunkLimit; i <= chunkLimit; i++)
		for (int j = -chunkLimit; j <= chunkLimit; j++)
		{
			ChunkPtr chunk = chunkMap[chunkIndex(i, j)];
			chunk->render();
		}

	if (saveShadowMapPpm)
	{
		light->savePpmFile();
		saveShadowMapPpm = false;
	}
}

void World::render(Shader groundShader, Shader skyShader, glm::vec3 camPos, glm::vec3 lookAt)
{
	// render skybox
	skyShader.use();
	skybox.render();

	// render ground
	groundShader.use();
	groundShader.setMat4("depthMVP", lights[0]->getDepthMVP());
	groundShader.setInt("depthMap", lights[0]->getShadowMapOnGPU());

	std::map<float, ChunkPtr> chunkWithTransparency;

	//int minI = std::max((int)std::round(camPos.x / chunkSize.x - renderRadius),-chunkLimit);
	//int maxI = std::min((int)std::round(camPos.x / chunkSize.x + renderRadius), chunkLimit);
	//int minJ = std::max((int)std::round(camPos.z / chunkSize.z - renderRadius),-chunkLimit);
	//int maxJ = std::min((int)std::round(camPos.z / chunkSize.z + renderRadius), chunkLimit);

	for (int i = -chunkLimit; i <= chunkLimit; i++)
		for (int j = -chunkLimit; j <= chunkLimit; j++)
		{
			ChunkPtr chunk = chunkMap[chunkIndex(i, j)];

			if (chunk->mustBeRegen())
				chunk->generateChunk();

			if (chunk->containsTransparentBlocks())
			{
				// put chunks containing transparency aside and sorted from farest to nearest vertex from the camera
				float distance = glm::length(glm::vec2(camPos.x, camPos.z) - glm::vec2(i*chunkSize.x, j*chunkSize.z));
				chunkWithTransparency[distance] = chunk;
			}
			// render solid chunk
			else
				chunk->render();
		}

	// render chunks containing transparency last
	for (std::map<float, ChunkPtr>::reverse_iterator it = chunkWithTransparency.rbegin(); it != chunkWithTransparency.rend(); ++it)
		it->second->render();
}

void World::clearBuffers()
{
	skybox.deleteVAO();
	skybox.clearVBOs();

	for (ChunkPtr chunk : chunkMap)
	{
		chunk->deleteVAO();
		chunk->clearVBOs();
	}
}

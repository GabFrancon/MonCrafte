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

void World::updateCurrentChunk(glm::vec3 camPos)
{
	int x = std::round(camPos.x / chunkSize.x);
	int y = std::round(camPos.z / chunkSize.z);
	if (x != currentChunk.x || y != currentChunk.y)
	{
		currentChunk = glm::ivec2(x, y);
		std::cout << "current chunk : (" << x << ", " << y << ")\n" << std::endl;
	}
}
glm::ivec2 World::toMapCoordinates(glm::ivec2 position)
{
	return glm::ivec2(std::round(position.x + worldSize / 2), std::round(position.y + worldSize / 2));
}

glm::ivec2 World::toWorldCoordinates(glm::ivec2 position)
{
	return glm::ivec2(std::round(position.x - worldSize / 2), std::round(position.y - worldSize / 2));
}


void World::addBlock(std::string texName)
{
	if (texName != "None")
	{
		bool transparency = (texName.back() == '+');
		chunkMap[0][0]->addBlock(getTexture(texName), transparency);
	}
}

void World::destroyBlock()
{
	chunkMap[0][0]->destroyBlock();
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

glm::ivec2 World::toGridCoord(glm::ivec2 chunkCoord)
{
	int x = chunkCoord.x + std::floor(worldSize / 2);
	int y = chunkCoord.y + std::floor(worldSize / 2);
	return glm::ivec2(x, y);
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
				for (int y = minY; y <= maxY; y++)
					if ((cam.y - 0.4 < y + 0.4) && (cam.y + 0.4 > y - 0.4))
					{
						glm::ivec2 associatedChunk = getAssociatedChunk(x, z);
						glm::ivec2 coords = toGridCoord(associatedChunk);
						if (!chunkMap[coords.x][coords.y]->canBeCrossed(toChunkCoord(glm::vec3(x, y, z), associatedChunk)))
							return true;
					}
	return false;
}

void World::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	chunkMap[0][0]->updateSelection(camPos, lookAt);
}

void World::genWorld()
{
	chunkMap[0][0] = std::make_shared<Chunk>(glm::ivec2(-1,-1), cube, textures["dirt"]);
	chunkMap[0][1] = std::make_shared<Chunk>(glm::ivec2(-1, 0), cube, textures["sand"]);
	chunkMap[1][0] = std::make_shared<Chunk>(glm::ivec2( 0,-1), cube, textures["sand"]);
	chunkMap[1][1] = std::make_shared<Chunk>(glm::ivec2( 0, 0), cube, textures["woodplanck"]);
	chunkMap[0][2] = std::make_shared<Chunk>(glm::ivec2(-1, 1), cube, textures["stone"]);
	chunkMap[2][0] = std::make_shared<Chunk>(glm::ivec2( 1,-1), cube, textures["gravel"]);
	chunkMap[1][2] = std::make_shared<Chunk>(glm::ivec2( 0, 1), cube, textures["sand"]);
	chunkMap[2][1] = std::make_shared<Chunk>(glm::ivec2( 1, 0), cube, textures["sand"]);
	chunkMap[2][2] = std::make_shared<Chunk>(glm::ivec2( 1, 1), cube, textures["dirt"]);

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
	for(int x=0 ; x<worldSize ; x++)
		for (int y = 0; y < worldSize; y++)
			if(chunkMap[x][y])
				chunkMap[x][y]->render(groundShader, camPos);
}

void World::clearBuffers()
{
	for (int x = 0; x < worldSize; x++)
		for (int y = 0; y < worldSize; y++)
			if (chunkMap[x][y])
				chunkMap[x][y]->clearBuffers();
}

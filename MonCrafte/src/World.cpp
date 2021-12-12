#include "World.h"

World::World(std::map<std::string, Texture> textureCollection, GLuint skyTexture) :
	cube(std::make_shared<Cube>()),
	textures(textureCollection),
	skybox(Skybox(skyTexture))
{
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

bool World::collide(glm::vec3 camPosition)
{
	return chunkMap[0][0]->collideGround(camPosition);
}

void World::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	chunkMap[0][0]->updateSelection(camPos, lookAt);
}

void World::genWorld()
{
	chunkMap[0][0] = std::make_shared<Chunk>(glm::ivec2(0, 0), cube, textures);
	chunkMap[0][1] = std::make_shared<Chunk>(glm::ivec2(0, 1), cube, textures);
	chunkMap[1][0] = std::make_shared<Chunk>(glm::ivec2(1, 0), cube, textures);
	chunkMap[1][1] = std::make_shared<Chunk>(glm::ivec2(1, 1), cube, textures);

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

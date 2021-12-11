#include "World.h"

World::World(std::map<std::string, Texture> textureCollection, GLuint skyTexture) :
	cube(std::make_shared<Cube>()),
	textures(textureCollection),
	skybox(Skybox(skyTexture))
{
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


void World::addBlock(std::string texName)
{
	if (texName != "None")
	{
		bool transparency = (texName.back() == '+');
		chunk.addBlock(getTexture(texName), transparency);
	}
}

void World::destroyBlock()
{
	chunk.destroyBlock();
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
	return chunk.collideGround(camPosition);
}

void World::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	chunk.updateSelection(camPos, lookAt);
}

void World::genWorld()
{
	chunk = Chunk(glm::ivec2(1, 0), cube, textures);

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
	chunk.render(groundShader, camPos);
}

void World::clearBuffers()
{
	chunk.clearBuffers();
}

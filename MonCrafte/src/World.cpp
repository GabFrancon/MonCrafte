#include "World.h"

World::World(std::map<std::string, GLuint> textureCollection) :
	cube(std::make_shared<Cube>()),
	sphere(std::make_shared<Sphere>()),
	textures(textureCollection) {}

void World::bindToGPU()
{
	cube->bindToGPU();
	sphere->bindToGPU();
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

void World::addLight(glm::vec3 position, float size, GLuint texture, glm::vec3 color)
{
	lights.push_back(std::make_shared<Light>(sphere, position, size, texture, color));
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
	chunk = Chunk(16, 30, 16, cube, textures);

	addLight(
		glm::vec3(20.0, 20.0, -20.0),						  // position
		1.0,												  // size
		textures["sun"],									  // texture
		glm::vec3(255.f / 255, 255.f / 255, 255.f / 255));    // color
}

void World::render(Shader shader, glm::vec3 camPos)
{
	for (int j = 0; j < lights.size(); j++)
		lights[j]->render(shader);

	chunk.render(shader, camPos);
}

void World::clearBuffers()
{
	for (int j = 0; j < lights.size(); j++)
		lights[j]->freeBuffer();

	chunk.clearBuffers();
}

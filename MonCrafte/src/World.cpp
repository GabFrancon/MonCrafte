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

void World::addBlock(glm::vec3 position, std::string texName)
{
	if (texName != "None")
	{
		bool transparency = (texName.back() == '+');
		ground.addBlock(position, getTexture(texName), getTexture("selection+"), transparency);
	}
}

void World::addBlock(std::string texName)
{
	if (texName != "None")
	{
		bool transparency = (texName.back() == '+');
		ground.addBlock(getTexture(texName), getTexture("selection+"), transparency);
	}
}

void World::destroyBlock()
{
	ground.destroyBlock();
}

void World::addLight(glm::vec3 position, float size, GLuint texture, glm::vec3 color)
{
	lights.push_back(std::make_shared<Light>(sphere, position, size, texture, color));
}

void World::destroyLight(unsigned int index)
{
	lights.erase(lights.begin() + index);
}

bool World::intersect(glm::vec3 camPosition)
{
	return ground.intersect(camPosition);
}

void World::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	ground.updateSelection(camPos, lookAt);
}

void World::genWorld()
{
	ground = Ground(10, 10, 10, cube);
	ground.genGround(textures);

	addLight(
		glm::vec3(20.0, 20.0, -20.0),						  // position
		1.0,												  // size
		textures["sun"],									  // texture
		glm::vec3(255.f / 255, 255.f / 255, 255.f / 255));    // color
}

void World::render(const GLuint program, glm::vec3 camPos)
{
	for (int j = 0; j < lights.size(); j++)
		lights[j]->render(program);

	ground.render(program, camPos);
}

void World::clearBuffers()
{
	for (int j = 0; j < lights.size(); j++)
		lights[j]->freeBuffer();

	ground.clearBuffers();
}

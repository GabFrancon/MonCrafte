#include "BiomeHelper.h"

BiomeHelper::BiomeHelper(std::map<std::string, Texture> textures, int worldSize, int chunkSize)
{
	this->worldSize = (int)worldSize / biomeSize;
	this->chunkSize = chunkSize;
	this->biomeLimit = (int)std::floor(this->worldSize / 2);
	this->textures = textures;
	biomeMap = std::vector<Biome>(this->worldSize * this->worldSize, Biome(BiomeType::PLAIN));
	biomeMap[biomeIndex(-1, -1)] = Biome(BiomeType::FOREST);
	biomeMap[biomeIndex(0, -1)] = Biome(BiomeType::FOREST);
	biomeMap[biomeIndex(1, -1)] = Biome(BiomeType::DESERT);
	biomeMap[biomeIndex(-1, 0)] = Biome(BiomeType::VALLEY);
	biomeMap[biomeIndex(0, 0)] = Biome(BiomeType::PLAIN);
	biomeMap[biomeIndex(1, 0)] = Biome(BiomeType::DESERT);
	biomeMap[biomeIndex(-1, 1)] = Biome(BiomeType::VALLEY);
	biomeMap[biomeIndex(0, 1)] = Biome(BiomeType::HILL);
	biomeMap[biomeIndex(1, 1)] = Biome(BiomeType::OCEAN);

	/*for(int i = 0 ; i < biomeMap.size() ; i++)
		biomeMap[i] = Biome(static_cast<BiomeType>(std::rand() % 5));*/
}

Texture BiomeHelper::getTexture(std::string name) { return textures[name]; }

int BiomeHelper::biomeIndex(int i, int j) { return i + biomeLimit + (j + biomeLimit) * worldSize; }


int BiomeHelper::getHeight(int x, int z, glm::ivec2 chunkPos)
{
	std::vector<float> params = interpolateHeight(x, z, chunkPos);
	return std::round(noise.get(x * increment, z * increment) * params[0] + params[1]);
}

BlockPtr BiomeHelper::genElement(glm::vec3 position, glm::ivec2 chunkPos)
{
	Biome biome = getBiomeLike(position.x, position.z, chunkPos);
	int height = getHeight(position.x, position.z, chunkPos);

	if (position.y < height - 4)
		return std::make_shared<Block>(position, getTexture("stone"));

	else if (position.y < height - 1)
		return std::make_shared<Block>(position, getTexture(biome.subTex));

	else if (position.y < height && height <= 0)
		return std::make_shared<Block>(position, getTexture("gravel"));

	else if (position.y < height && height > 0)
		return std::make_shared<Block>(position, getTexture(biome.mainTex));

	else if (position.y <= 0)
		return std::make_shared<Block>(position, getTexture("water"));

	return std::make_shared<Block>(position, Texture());
}

std::vector<float> BiomeHelper::interpolateHeight(int x, int z, glm::ivec2 chunkPos)
{
	Biome biome = getRelatedBiome(chunkPos);
	Biome leftBiome = getRelatedBiome(chunkPos + glm::ivec2(-1, 0));
	Biome rightBiome = getRelatedBiome(chunkPos + glm::ivec2(1, 0));
	Biome backBiome = getRelatedBiome(chunkPos + glm::ivec2(0, -1));
	Biome frontBiome = getRelatedBiome(chunkPos + glm::ivec2(0, 1));
	Biome topLeftBiome = getRelatedBiome(chunkPos + glm::ivec2(-1, 1));
	Biome topRightBiome = getRelatedBiome(chunkPos + glm::ivec2(1, 1));
	Biome bottomLeftBiome = getRelatedBiome(chunkPos + glm::ivec2(-1, -1));
	Biome bottomRightBiome = getRelatedBiome(chunkPos + glm::ivec2(1, -1));

	float deltaX = (x - chunkSize * chunkPos.x + std::floor((float)chunkSize / 2)) / chunkSize;
	float deltaZ = (z - chunkSize * chunkPos.y + std::floor((float)chunkSize / 2)) / chunkSize;

	float amplitude = (
		  deltaX * rightBiome.amplitude + (1 - deltaX) * leftBiome.amplitude
		+ deltaZ * frontBiome.amplitude + (1 - deltaZ) * backBiome.amplitude
		+ (bottomLeftBiome.amplitude * (1 - deltaX) + bottomRightBiome.amplitude * deltaX) * (1 - deltaZ)
		+ (topLeftBiome.amplitude * (1 - deltaX) + topRightBiome.amplitude * deltaX) * deltaZ) / 3;

	float offset = (
		  deltaX * rightBiome.offset + (1 - deltaX) * leftBiome.offset
		+ deltaZ * frontBiome.offset + (1 - deltaZ) * backBiome.offset
		+ (bottomLeftBiome.offset * (1 - deltaX) + bottomRightBiome.offset * deltaX) * (1 - deltaZ)
		+ (topLeftBiome.offset * (1 - deltaX) + topRightBiome.offset * deltaX) * deltaZ) / 3;

	amplitude = (1.5 * amplitude + 0.5 * biome.amplitude) / 2;
	offset = (1.5 * offset + 0.5 * biome.offset) / 2;

	return std::vector<float>{amplitude, offset};
}

Biome BiomeHelper::getBiomeLike(int x, int z, glm::ivec2 chunkPos)
{
	int random = std::rand() % 10;
	if (random < 8)
		return getRelatedBiome(chunkPos);

	float deltaX = (x - chunkSize * chunkPos.x + std::floor((float)chunkSize / 2)) / chunkSize;
	float deltaZ = (z - chunkSize * chunkPos.y + std::floor((float)chunkSize / 2)) / chunkSize;

	std::vector<float> dist = { deltaX, 1 - deltaX, deltaZ, 1 - deltaZ };
	auto minD = std::min_element(dist.begin(), dist.end());
	int nearestBiome = std::distance(dist.begin(), minD);

	switch (nearestBiome)
	{
	case 0:
		return getRelatedBiome(chunkPos + glm::ivec2(-1, 0));
		break;
	case 1:
		return getRelatedBiome(chunkPos + glm::ivec2(1, 0));
		break;
	case 2:
		return getRelatedBiome(chunkPos + glm::ivec2(0, -1));
		break;
	default:
		return getRelatedBiome(chunkPos + glm::ivec2(0, 1));
		break;
	}
}

Biome BiomeHelper::getRelatedBiome(glm::ivec2 chunkPos)
{
	int biomeX = std::floor((chunkPos.x + (float)biomeSize / 2) / biomeSize);
	while (biomeX < -biomeLimit)
		biomeX++;
	while (biomeX > biomeLimit)
		biomeX--;

	int biomeZ = std::floor((chunkPos.y + (float)biomeSize / 2) / biomeSize);
	while (biomeZ < -biomeLimit)
		biomeZ++;
	while (biomeZ > biomeLimit)
		biomeZ--;

	return biomeMap[biomeIndex(biomeX, biomeZ)];
}
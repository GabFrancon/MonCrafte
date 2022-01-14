#include "Shader.h"
#include "PerlinNoise.h"
#include "Block.h"

enum class BiomeType { OCEAN, PLAIN, FOREST, DESERT, VALLEY, HILL, TRANSITION };

struct Biome
{
	BiomeType type = BiomeType::PLAIN;
	float maxHeight;
	float offset;
	int treeDensity;
	std::string mainTex, subTex;

	Biome(BiomeType type)
	{
		// default parameters
		this->type = type;
		this->maxHeight = 5;
		this->offset = 3;
		this->treeDensity = 0;
		this->mainTex = "grass";
		this->subTex = "dirt";

		switch (type)
		{
		case BiomeType::OCEAN:
			maxHeight = 10;
			offset = -15;
			break;
		case BiomeType::PLAIN:
			maxHeight = 5;
			offset = 3;
			break;
		case BiomeType::FOREST:
			maxHeight = 5;
			offset = 3;
			treeDensity = 16;
			break;
		case BiomeType::DESERT:
			maxHeight = 6;
			offset = 4;
			mainTex = "sand";
			subTex = "sand";
			break;
		case BiomeType::VALLEY:
			maxHeight = 10;
			offset = 5;
			treeDensity = 3;
			break;
		case BiomeType::HILL:
			maxHeight = 25;
			offset = 12;
			treeDensity = 2;
			break;
		}
	}
};

class BiomeHelper
{
public:
	BiomeHelper() {}

	BiomeHelper(std::map<std::string, Texture> textures, int worldSize = 15, int chunkSize = 15)
	{
		this->worldSize = (int)worldSize / biomeSize;
		this->chunkSize = chunkSize;
		this->biomeLimit = (int)std::floor(this->worldSize / 2);
		this->textures = textures;
		biomeMap = std::vector<Biome>(this->worldSize * this->worldSize, Biome(BiomeType::PLAIN));
		biomeMap[biomeIndex(-1,-1)] = Biome(BiomeType::FOREST);
		biomeMap[biomeIndex( 0,-1)] = Biome(BiomeType::FOREST);
		biomeMap[biomeIndex( 1,-1)] = Biome(BiomeType::DESERT);
		biomeMap[biomeIndex(-1, 0)] = Biome(BiomeType::VALLEY);
		biomeMap[biomeIndex( 0, 0)] = Biome(BiomeType::PLAIN);
		biomeMap[biomeIndex( 1, 0)] = Biome(BiomeType::DESERT);
		biomeMap[biomeIndex(-1, 1)] = Biome(BiomeType::VALLEY);
		biomeMap[biomeIndex( 0, 1)] = Biome(BiomeType::HILL);
		biomeMap[biomeIndex( 1, 1)] = Biome(BiomeType::OCEAN);
	}

	Texture getTexture(std::string name) { return textures[name]; }

	int getHeight(int x, int z, glm::ivec2 chunkPos)
	{
		std::vector<int> params = interpolate(x, z, chunkPos);
		return noise.get(x * increment, z * increment) * params[0] + params[1];
	}

	BlockPtr genElement(glm::vec3 position, glm::ivec2 chunkPos)
	{
		Biome biome = getRelatedBiome(chunkPos);
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

	std::vector<int> interpolate(int x, int z, glm::ivec2 chunkPos)
	{
		Biome leftBiome  = getRelatedBiome(chunkPos - glm::ivec2(1, 0));
		Biome rightBiome = getRelatedBiome(chunkPos + glm::ivec2(1, 0));
		Biome backBiome  = getRelatedBiome(chunkPos - glm::ivec2(0, 1));
		Biome frontBiome = getRelatedBiome(chunkPos + glm::ivec2(0, 1));

		float deltaX = (x - chunkSize * chunkPos.x + std::floor((float)chunkSize / 2)) / chunkSize;
		float deltaZ = (z - chunkSize * chunkPos.y + std::floor((float)chunkSize / 2)) / chunkSize;

		int amplitude = std::round((deltaX * rightBiome.maxHeight + (1 - deltaX) * leftBiome.maxHeight + deltaZ * frontBiome.maxHeight + (1 - deltaZ) * backBiome.maxHeight) / 2);
		int offset    = std::round((deltaX * rightBiome.offset    + (1 - deltaX) * leftBiome.offset    + deltaZ * frontBiome.offset    + (1 - deltaZ) * backBiome.offset)    / 2);

		return std::vector<int>{amplitude, offset};
	}

	Biome getRelatedBiome(glm::ivec2 chunkPos)
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

	int biomeIndex(int i, int j)
	{
		return i + biomeLimit + (j + biomeLimit) * worldSize;
	}

private:
	PerlinNoise noise{ 7, 4, 1, 93 };

	std::vector<Biome> biomeMap;
	std::map<std::string, Texture> textures;

	int biomeLimit = 0;
	int worldSize = 0;	// in terms of numbr of biomes
	int biomeSize = 5;  // in terms of number of chunks
	int chunkSize = 0;  // in terms of number of blocks
	float increment = 0.01;
};


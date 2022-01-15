#include "utils.h"
#include "PerlinNoise.h"
#include "Block.h"

enum class BiomeType { OCEAN, PLAIN, FOREST, DESERT, VALLEY, HILL};

struct Biome
{
	BiomeType type = BiomeType::PLAIN;
	float amplitude;
	float offset;
	int treeDensity;
	std::string mainTex, subTex;

	Biome(BiomeType type)
	{
		// default parameters
		this->type = type;
		this->amplitude = 5;
		this->offset = 3;
		this->treeDensity = 0;
		this->mainTex = "grass";
		this->subTex = "dirt";

		switch (type)
		{
		case BiomeType::OCEAN:
			amplitude = 7;
			offset = -15;
			break;
		case BiomeType::PLAIN:
			amplitude = 5;
			offset = 3;
			break;
		case BiomeType::FOREST:
			amplitude = 5;
			offset = 3;
			treeDensity = 16;
			break;
		case BiomeType::DESERT:
			amplitude = 5;
			offset = 4;
			mainTex = "sand";
			subTex = "sand";
			break;
		case BiomeType::VALLEY:
			amplitude = 9;
			offset = 7;
			treeDensity = 2;
			break;
		case BiomeType::HILL:
			amplitude = 25;
			offset = 16;
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
		/*biomeMap[biomeIndex(-1,-1)] = Biome(BiomeType::FOREST);
		biomeMap[biomeIndex( 0,-1)] = Biome(BiomeType::FOREST);
		biomeMap[biomeIndex( 1,-1)] = Biome(BiomeType::DESERT);
		biomeMap[biomeIndex(-1, 0)] = Biome(BiomeType::VALLEY);
		biomeMap[biomeIndex( 0, 0)] = Biome(BiomeType::PLAIN);
		biomeMap[biomeIndex( 1, 0)] = Biome(BiomeType::DESERT);
		biomeMap[biomeIndex(-1, 1)] = Biome(BiomeType::VALLEY);
		biomeMap[biomeIndex( 0, 1)] = Biome(BiomeType::HILL);
		biomeMap[biomeIndex( 1, 1)] = Biome(BiomeType::OCEAN);*/
		for(int i = 0 ; i < biomeMap.size() ; i++)
			biomeMap[i] = Biome(static_cast<BiomeType>(std::rand() % 5));
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
		Biome biome = getRelatedBiome(chunkPos);

		Biome leftBiome  = getRelatedBiome(chunkPos + glm::ivec2(-1, 0));
		Biome rightBiome = getRelatedBiome(chunkPos + glm::ivec2( 1, 0));
		Biome backBiome  = getRelatedBiome(chunkPos + glm::ivec2( 0,-1));
		Biome frontBiome = getRelatedBiome(chunkPos + glm::ivec2( 0, 1));

		Biome topLeftBiome     = getRelatedBiome(chunkPos + glm::ivec2(-1, 1));
		Biome topRightBiome    = getRelatedBiome(chunkPos + glm::ivec2( 1, 1));
		Biome bottomLeftBiome  = getRelatedBiome(chunkPos + glm::ivec2(-1,-1));
		Biome bottomRightBiome = getRelatedBiome(chunkPos + glm::ivec2( 1,-1));

		float deltaX = (x - chunkSize * chunkPos.x + std::floor((float)chunkSize / 2)) / chunkSize;
		float deltaZ = (z - chunkSize * chunkPos.y + std::floor((float)chunkSize / 2)) / chunkSize;

		int amplitude = (
			  deltaX * rightBiome.amplitude + (1 - deltaX) * leftBiome.amplitude
			+ deltaZ * frontBiome.amplitude + (1 - deltaZ) * backBiome.amplitude
			+ (bottomLeftBiome.amplitude * (1 - deltaX) + bottomRightBiome.amplitude * deltaX) * (1 - deltaZ)
			+ (topLeftBiome.amplitude * (1 - deltaX) + topRightBiome.amplitude * deltaX) * deltaZ) / 3;

		int offset = (
			  deltaX * rightBiome.offset    + (1 - deltaX) * leftBiome.offset
			+ deltaZ * frontBiome.offset    + (1 - deltaZ) * backBiome.offset
			+ (bottomLeftBiome.offset * (1 - deltaX) + bottomRightBiome.offset * deltaX) * (1 - deltaZ) 
			+ (topLeftBiome.offset * (1 - deltaX) + topRightBiome.offset * deltaX) * deltaZ)    / 3;
		
		amplitude = (1.5 * amplitude + 0.5 * biome.amplitude) / 2;
		offset    = (1.5 * offset    + 0.5 * biome.offset)    / 2;

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


#include "Shader.h"
#include "PerlinNoise.h"

enum class BiomeType { OCEAN, PLAIN, VALLEY, HILL, TRANSITION };

struct Biome
{
	BiomeType type = BiomeType::PLAIN;
	float maxHeight;
	float offset;

	Biome(BiomeType type)
	{
		this->type = type;
		this->maxHeight = 5;
		this->offset = 2;

		switch (type)
		{
		case BiomeType::OCEAN:
			maxHeight = 10;
			offset = -10;
			break;
		case BiomeType::PLAIN:
			maxHeight = 5;
			offset = 2;
			break;
		case BiomeType::VALLEY:
			maxHeight = 15;
			offset = 5;
			break;
		case BiomeType::HILL:
			maxHeight = 22;
			offset = 10;
			break;
		}
	}
};

class BiomeHelper
{
public:
	BiomeHelper(int worldSize = 15, int chunkSize = 15)
	{
		this->worldSize = (int)worldSize / biomeSize;
		this->chunkSize = chunkSize;
		this->biomeLimit = (int)std::floor(this->worldSize / 2);
		biomeMap = std::vector<Biome>(this->worldSize * this->worldSize, Biome(BiomeType::PLAIN));
		biomeMap[biomeIndex(0, 0)] = Biome(BiomeType::HILL);
	}

	int getHeight(int x, int z, glm::ivec2 chunkPos)
	{
		glm::ivec2 biomePos = getRelatedBiome(chunkPos);
		Biome biome = biomeMap[biomeIndex(biomePos.x, biomePos.y)];

		// by default (chunk not on frontiere), simply use constant parameters of current biome
		float amplitude = biome.maxHeight;
		int offset = biome.offset;

		// smoothing left to right frontiere between biomes
		if (biomePos.x > -biomeLimit)
		{
			glm::ivec2 left = getRelatedBiome(chunkPos - glm::ivec2(1, 0));
			Biome leftBiome = biomeMap[biomeIndex(left.x, left.y)];
			if (leftBiome.type != biome.type)
			{
				int relativeX = x - chunkSize * chunkPos.x + std::floor((float)chunkSize / 2);
				float coeff = (float)relativeX / chunkSize;

				amplitude = coeff * amplitude + (1 - coeff) * leftBiome.maxHeight;
				offset = coeff * offset + (1 - coeff) * leftBiome.offset;
			}
		}
		// smoothing back to front frontiere between biomes
		if (biomePos.y > -biomeLimit)
		{
			glm::ivec2 back = getRelatedBiome(chunkPos - glm::ivec2(0, 1));
			Biome backBiome = biomeMap[biomeIndex(back.x, back.y)];
			if (backBiome.type != biome.type)
			{
				int relativeZ = z - chunkSize * chunkPos.y + std::floor((float)chunkSize / 2);
				float coeff = (float)relativeZ / chunkSize;

				amplitude = coeff * amplitude + (1 - coeff) * backBiome.maxHeight;
				offset = coeff * offset + (1 - coeff) * backBiome.offset;
			}
		}

		return noise.get(x * increment, z * increment) * amplitude + offset;
	}

	glm::ivec2 getRelatedBiome(glm::ivec2 chunkPos)
	{
		int biomeX = std::floor((chunkPos.x + (float)biomeSize / 2) / biomeSize);
		int biomeZ = std::floor((chunkPos.y + (float)biomeSize / 2) / biomeSize);
		return glm::ivec2(biomeX, biomeZ);
	}
	int biomeIndex(int i, int j)
	{
		return i + biomeLimit + (j + biomeLimit) * worldSize;
	}

private:
	PerlinNoise noise{ 7, 4, 1, 93 };
	std::vector<Biome> biomeMap;
	int biomeLimit = 0;
	int worldSize = 0;	// in terms of numbr of biomes
	int biomeSize = 5;  // in terms of number of chunks
	int chunkSize = 0;  // in terms of number of blocks
	float increment = 0.01;
};


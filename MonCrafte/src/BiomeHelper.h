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
			treeDensity = 20;
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
	BiomeHelper(std::map<std::string, Texture> textures, int worldSize = 15, int chunkSize = 15);

	Texture getTexture(std::string name);
	int biomeIndex(int i, int j);
	int getHeight(int x, int z, glm::ivec2 chunkPos);

	BlockPtr genElement(glm::vec3 position, glm::ivec2 chunkPos);
	std::vector<float> interpolateHeight(int x, int z, glm::ivec2 chunkPos);
	Biome getBiomeLike(int x, int z, glm::ivec2 chunkPos);
	Biome getRelatedBiome(glm::ivec2 chunkPos);


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


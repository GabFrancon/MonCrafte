#include "Chunk.h"

Chunk::Chunk(glm::ivec2 position, CubePtr geometry, Texture texture) :
	chunkPos(position),
	isSelection(false),
	selectedFace(-1),
	distanceToSelection(0.0f)
{
	selection = toWorldCoord(glm::ivec3(0, 0, 0));

	map = std::vector<std::vector<std::vector<BlockPtr>>>(chunkSize.x, std::vector<std::vector<BlockPtr>>(chunkSize.y, std::vector<BlockPtr>(chunkSize.z, nullptr)));

	for (int x = 0; x < chunkSize.x; x++) {
		for (int y = 0; y < chunkSize.y; y++) {
			for (int z = 0; z < chunkSize.z; z++) {
				glm::ivec3 coords = glm::ivec3(x, y, z);
				if (y < 5)
					map[x][y][z] = std::make_shared<Block>(Type::SOLID, geometry, toWorldCoord(coords), texture);
				else
					map[x][y][z] = std::make_shared<Block>(Type::AIR, geometry, toWorldCoord(coords), Texture());
				}}}

	for (int x = 0; x < chunkSize.x; x++) {
		for (int y = 0; y < chunkSize.y; y++) {
			for (int z = 0; z < chunkSize.z; z++)
				hideNeighboursFace(map[x][y][z]);
		}}
}

glm::ivec3 Chunk::toChunkCoord(glm::vec3 pos)
{
	int x = pos.x - chunkSize.x * chunkPos.x + std::floor((float)chunkSize.x/2);
	int y = pos.y                            + std::floor((float)chunkSize.y/2);
	int z = pos.z - chunkSize.z * chunkPos.y + std::floor((float)chunkSize.z/2);
	return glm::ivec3(x, y, z);
}

glm::vec3 Chunk::toWorldCoord(glm::ivec3 coords)
{
	float x = coords.x + chunkSize.x * chunkPos.x - std::floor((float)chunkSize.x/2);
	float y = coords.y                            - std::floor((float)chunkSize.y/2);
	float z = coords.z + chunkSize.z * chunkPos.y - std::floor((float)chunkSize.z/2);
	return glm::vec3(x, y, z);
}


bool Chunk::isInChunk(glm::ivec3 coords)
{
	if (0 <= coords.x && coords.x < chunkSize.x)
		if (0 <= coords.y && coords.y < chunkSize.y)
			if (0 <= coords.z && coords.z < chunkSize.z)
				return true;
	return false;
}

void Chunk::addBlock(Texture texture, bool transparency)
{
	if (isSelection && distanceToSelection > 1.5f)
	{
		glm::vec3  position = selection;
		switch (selectedFace)
		{
		case 0:position += glm::vec3(0.0, 0.0, 1.0); break;
		case 1:position -= glm::vec3(0.0, 0.0, 1.0); break;
		case 2:position += glm::vec3(0.0, 1.0, 0.0); break;
		case 3:position -= glm::vec3(0.0, 1.0, 0.0); break;
		case 4:position += glm::vec3(1.0, 0.0, 0.0); break;
		case 5:position -= glm::vec3(1.0, 0.0, 0.0); break;
		default: break;
		}
		glm::ivec3 coords = toChunkCoord(position);
		if (isInChunk(coords))
		{
			map[coords.x][coords.y][coords.z]->fillObject(texture, transparency);
			hideNeighboursFace(map[coords.x][coords.y][coords.z]);
		}
	}
}

void Chunk::selectObject(glm::vec3 position)
{
	glm::ivec3 old = toChunkCoord(selection);
	map[old.x][old.y][old.z]->setPointed(false);

	if (isSelection)
	{
		glm::ivec3 coords = toChunkCoord(position);
		map[coords.x][coords.y][coords.z]->setPointed(true);

		selection = position;
	}
}

void Chunk::destroyBlock()
{
	if (isSelection)
	{
		glm::ivec3 coords = toChunkCoord(selection);
		if (isInChunk(coords))
		{
			map[coords.x][coords.y][coords.z]->emptyObject();
			showNeighboursFace(map[coords.x][coords.y][coords.z]);
		}
	}
}

bool Chunk::canBeCrossed(glm::ivec3 blockCoords)
{
	BlockPtr object = map[blockCoords.x][blockCoords.y][blockCoords.z];
	return object->isEmpty() || object->isTransparent() || object->isHidden();
}

float Chunk::faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal)
{
	float dist = 10;
	if (glm::dot(lookAt, normal) != 0)
	{
		float iDist = glm::dot(point - camPos, normal) / glm::dot(lookAt, normal);

		if (0 < iDist)
		{
			bool abscissa = false;
			bool ordinate = false;
			glm::vec3 iPoint = camPos + iDist * lookAt;

			if (normal.x != 0)
			{
				abscissa = (point.y - 0.5 < iPoint.y) && (iPoint.y < point.y + 0.5);
				ordinate = (point.z - 0.5 < iPoint.z) && (iPoint.z < point.z + 0.5);
			}
			else if (normal.y != 0)
			{
				abscissa = (point.x - 0.5 < iPoint.x) && (iPoint.x < point.x + 0.5);
				ordinate = (point.z - 0.5 < iPoint.z) && (iPoint.z < point.z + 0.5);
			}
			else if (normal.z != 0)
			{
				abscissa = (point.x - 0.5 < iPoint.x) && (iPoint.x < point.x + 0.5);
				ordinate = (point.y - 0.5 < iPoint.y) && (iPoint.y < point.y + 0.5);
			}
			if (abscissa && ordinate)
				dist = iDist;
		}
	}
	return dist;
}

std::map<std::string, BlockPtr> Chunk::getNeighbours(BlockPtr block)
{
	glm::ivec3 coords = toChunkCoord(block->getPosition());
	int x = coords.x;
	int y = coords.y;
	int z = coords.z;
	std::map<std::string, BlockPtr> neighbours;

	if (x < chunkSize.x - 1)
		neighbours["left"]   = map[x+1][y  ][z  ];
	if (x > 0)
		neighbours["right"]  = map[x-1][y  ][z  ];
	if (y < chunkSize.y - 1)
		neighbours["bottom"] = map[x  ][y+1][z  ];
	if (y > 0)
		neighbours["top"]    = map[x  ][y-1][z  ];
	if (z < chunkSize.z - 1)
		neighbours["back"]   = map[x  ][y  ][z+1];
	if (z > 0)
		neighbours["front"]  = map[x  ][y  ][z-1];

	return neighbours;
}

void Chunk::hideNeighboursFace(BlockPtr block)
{
	if (!block->isEmpty())
	{
		std::map<std::string, BlockPtr> neighbours = getNeighbours(block);

		for (auto& it : neighbours)
		{
			it.second->setFaceRendering(it.first, block->isTransparent() && !it.second->isTransparent() && !it.second->isEmpty());
			block->setFaceRendering(block->getReversedFace(it.first), (!block->isTransparent() && it.second->isTransparent()) || it.second->isEmpty());
		}
	}	
}

void Chunk::showNeighboursFace(BlockPtr block)
{
	if (block->isTransparent() || block->isEmpty())
	{
		std::map<std::string, BlockPtr> neighbours = getNeighbours(block);

		for (auto& it : neighbours)
			it.second->setFaceRendering(it.first, !it.second->isEmpty());
	}
}

void Chunk::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	bool found = false;
	glm::vec3 nearestCube;
	int nearestFace = -1;
	float minDist = 10;

	glm::ivec3 coords = toChunkCoord(camPos);

	int minX = (lookAt.x >=0) * coords.x + (lookAt.x < 0) * std::max(coords.x - 5, 0);
	int maxX = (lookAt.x < 0) * coords.x + (lookAt.x >=0) * std::min(coords.x + 5, chunkSize.x - 1);
	int minY = (lookAt.y >= 0) * coords.y + (lookAt.y < 0) * std::max(coords.y - 5, 0);
	int maxY = (lookAt.y < 0) * coords.y + (lookAt.y >= 0) * std::min(coords.y + 5, chunkSize.y - 1);
	int minZ = (lookAt.z >= 0) * coords.z + (lookAt.z < 0) * std::max(coords.z - 5, 0);
	int maxZ = (lookAt.z < 0) * coords.z + (lookAt.z >= 0) * std::min(coords.z + 5, chunkSize.z - 1);

	for (int x = minX ; x <= maxX ; x++) {
		for (int y = minY; y <= maxY; y++) {
			for (int z = minZ; z <= maxZ ; z++)
			{
				if (!map[x][y][z]->isEmpty() && !map[x][y][z]->isHidden())
				{
					glm::vec3 pos = map[x][y][z]->getPosition();
					float frontDist  = faceDistance(camPos, lookAt, pos + glm::vec3(0.0, 0.0, 0.5), glm::vec3( 0.0, 0.0, 1.0));
					float backDist   = faceDistance(camPos, lookAt, pos - glm::vec3(0.0, 0.0, 0.5), glm::vec3( 0.0, 0.0,-1.0));
					float topDist    = faceDistance(camPos, lookAt, pos + glm::vec3(0.0, 0.5, 0.0), glm::vec3( 0.0, 1.0, 0.0));
					float bottomDist = faceDistance(camPos, lookAt, pos - glm::vec3(0.0, 0.5, 0.0), glm::vec3( 0.0,-1.0, 0.0));
					float rightDist  = faceDistance(camPos, lookAt, pos + glm::vec3(0.5, 0.0, 0.0), glm::vec3( 1.0, 0.0, 0.0));
					float leftDist   = faceDistance(camPos, lookAt, pos - glm::vec3(0.5, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0));
					std::vector<float> dist{ frontDist, backDist, topDist, bottomDist, rightDist, leftDist };

					auto minD = std::min_element(dist.begin(), dist.end());
					if (*minD < minDist)
					{
						found = true;
						minDist = *minD;
						nearestCube = pos;
						nearestFace = std::distance(dist.begin(), minD);
					}
				}
			}
		}
	}
	isSelection = found;
	selectObject(nearestCube);
	selectedFace = nearestFace;
	distanceToSelection = minDist;
}

void Chunk::render(Shader shader, glm::vec3 camPos)
{
	shader.use();
	int count = 0;
	std::vector<BlockPtr> transparentBlocks;
	for (int x = 0; x < chunkSize.x; x++)
	{
		for (int y = 0; y < chunkSize.y; y++)
		{
			for (int z = 0; z < chunkSize.z; z++)
			{	
				if (map[x][y][z]->isTransparent())
					transparentBlocks.push_back(map[x][y][z]);
				else
					map[x][y][z]->render(shader);
			}
		}
	}
	if (transparentBlocks.size() != 0)
	{
		std::map<float, BlockPtr> sorted;
		for (BlockPtr block : transparentBlocks)
		{
			float distance = glm::length(camPos - block->getPosition());
			sorted[distance] = block;
		}
		for (std::map<float, BlockPtr>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
			it->second->render(shader);
	}
	
}

void Chunk::clearBuffers()
{
	for (int x = 0; x < chunkSize.x; x++)
	{
		for (int y = 0; y < chunkSize.y; y++)
		{
			for (int z = 0; z < chunkSize.z; z++)
			{
				map[x][y][z]->freeBuffer();
			}
		}
	}
}
#include "Ground.h"

Ground::Ground(int width, int height, int length, CubePtr cube, std::map<std::string, GLuint> textures) :
	halfWidth(std::round(width / 2)),
	halfHeight(std::round(height / 2)),
	halfLength(std::round(length / 2)),
	isSelection(false),
	selectedFace(-1),
	distanceToSelection(0.0f)
{
	map = std::vector<std::vector<std::vector<BlockPtr>>>(width, std::vector<std::vector<BlockPtr>>(height, std::vector<BlockPtr>(length, nullptr)));

	for (int x = 0; x < 2 * halfWidth; x++)
	{
		for (int y = 0; y < 2 * halfHeight; y++)
		{
			for (int z = 0; z < 2 * halfLength; z++)
			{
				if (y == 0)
					map[x][y][z] = std::make_shared<Block>(cube, toWorldCoordinates(x, y, z), 1.0, textures["stone"], textures["selection+"], false);
				else
					map[x][y][z] = std::make_shared<Block>(cube, toWorldCoordinates(x, y, z), textures["selection+"]);
			}
		}
	}
}

void Ground::addBlock(glm::vec3 position, GLuint texture, bool transparency)
{
	if (isInMap(position))
	{
		std::vector<int> coords = toMapCoordinates(position);
		map[coords[0]][coords[1]][coords[2]]->fillObject(texture, transparency);
		setNeighbourRender(map[coords[0]][coords[1]][coords[2]]);
	}
}

void Ground::addBlock(GLuint texture, bool transparency)
{
	if (isSelection)
	{
		glm::vec3  position = selection;
		switch (selectedFace)
		{
		case 0:
			position += glm::vec3(0.0, 0.0, 1.0);
			break;
		case 1:
			position -= glm::vec3(0.0, 0.0, 1.0);
			break;
		case 2:
			position += glm::vec3(0.0, 1.0, 0.0);
			break;
		case 3:
			position -= glm::vec3(0.0, 1.0, 0.0);
			break;
		case 4:
			position += glm::vec3(1.0, 0.0, 0.0);
			break;
		case 5:
			position -= glm::vec3(1.0, 0.0, 0.0);
			break;
		default:
			break;
		}
		addBlock(position, texture, transparency);
	}
}

void Ground::selectObject(glm::vec3 position)
{
	std::vector<int> old = toMapCoordinates(selection);
	map[old[0]][old[1]][old[2]]->setPointed(false);

	if (isSelection)
	{
		std::vector<int> coords = toMapCoordinates(position);
		map[coords[0]][coords[1]][coords[2]]->setPointed(true);

		selection = position;
	}
}

void Ground::destroyBlock()
{
	if (isSelection && isInMap(selection))
	{
		std::vector<int> coords = toMapCoordinates(selection);
		map[coords[0]][coords[1]][coords[2]]->emptyObject();

		std::vector<BlockPtr> neighbours = getNeighbours(map[coords[0]][coords[1]][coords[2]]);
		int count = 0;

		for (BlockPtr block : neighbours)
		{
			if (!block->isEmpty())
			{
				block->setRendering(true);
				count++;
			}
		}
		std::cout << "(del)" << count << " neighbours rendered\n" << std::endl;
	}
}

bool Ground::isInMap(glm::vec3 position)
{
	bool xAxis = -halfWidth <= position.x && position.x < halfWidth;
	bool yAxis = -halfHeight <= position.y && position.y < halfHeight;
	bool zAxis = -halfLength <= position.z && position.z < halfLength;
	return xAxis && yAxis && zAxis;
}

bool Ground::isOnBorders(glm::vec3 position)
{
	bool xAxis = -halfWidth == position.x || position.x == halfWidth - 1;
	bool yAxis = -halfHeight == position.y || position.y == halfHeight - 1;
	bool zAxis = -halfLength == position.z || position.z == halfLength - 1;
	return xAxis || yAxis || zAxis;
}

bool Ground::collideObject(BlockPtr object, glm::vec3 position)
{
	if (object->isEmpty() || object->isTransparent())
		return false;

	glm::vec3 objPos = object->getPosition();
	bool xAxis = (position.x - 0.4 < objPos.x + 0.4) && (position.x + 0.4 > objPos.x - 0.4);
	bool yAxis = (position.y - 1.0 < objPos.y + 1.0) && (position.y + 1.0 > objPos.y - 1.0);
	bool zAxis = (position.z - 0.4 < objPos.z + 0.4) && (position.z + 0.4 > objPos.z - 0.4);

	return xAxis && yAxis && zAxis;

}

bool Ground::collideGround(glm::vec3 cam)
{
	std::vector<int> coords = toMapCoordinates(cam);
	int minX = std::max(coords[0] - 2, 0);
	int maxX = std::min(coords[0] + 3, 2 * halfWidth);
	int minY = std::max(coords[1] - 2, 0);
	int maxY = std::min(coords[1] + 3, 2 * halfHeight);
	int minZ = std::max(coords[2] - 2, 0);
	int maxZ = std::min(coords[2] + 3, 2 * halfLength);

	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			for (int z = minZ; z < maxZ; z++)
			{
				if (collideObject(map[x][y][z], cam))
					return true;
			}
		}
	}
	return false;
}

float Ground::faceDistance(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 point, glm::vec3 normal)
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

std::vector<BlockPtr> Ground::getNeighbours(BlockPtr block)
{
	std::vector<int> coords = toMapCoordinates(block->getPosition());
	int x = coords[0];
	int y = coords[1];
	int z = coords[2];
	std::vector<BlockPtr> neighbours;

	if (x < 2 * halfWidth - 1)
		neighbours.push_back(map[x + 1][y][z]);
	if (x > 0)
		neighbours.push_back(map[x - 1][y][z]);
	if (y < 2 * halfHeight - 1)
		neighbours.push_back(map[x][y + 1][z]);
	if (y > 0)
		neighbours.push_back(map[x][y - 1][z]);
	if (z < 2 * halfLength - 1)
		neighbours.push_back(map[x][y][z + 1]);
	if (z > 0)
		neighbours.push_back(map[x][y][z - 1]);

	return neighbours;
}


void Ground::setNeighbourRender(BlockPtr block)
{
	std::vector<BlockPtr> neighbours = getNeighbours(block);
	int j = 0;

	for (BlockPtr neighbour : neighbours)
	{
		if (!neighbour->isEmpty())
		{
			if (isOnBorders(neighbour->getPosition()) || block->isTransparent())
			{
				neighbour->setRendering(true);
				j++;
			}
			else
			{
				std::vector<BlockPtr> n = getNeighbours(neighbour);
				neighbour->setRendering(false);
				int i = 0;
				while (i < n.size())
				{
					if (n[i]->isEmpty() || n[i]->isTransparent())
					{
						neighbour->setRendering(true);
						j++;
						break;
					}
					i++;
				}
			}
		}
	}
	std::cout << "(add)" << j << " neighbours rendered\n" << std::endl;
}


void Ground::updateSelection(glm::vec3 camPos, glm::vec3 lookAt)
{
	bool found = false;
	glm::vec3 nearestCube;
	int nearestFace = -1;
	float minDist = 10;

	std::vector<int> coords = toMapCoordinates(camPos);
	int minX = std::max(coords[0] - 5, 0);
	int maxX = std::min(coords[0] + 6, 2 * halfWidth);
	int minY = std::max(coords[1] - 5, 0);
	int maxY = std::min(coords[1] + 6, 2 * halfHeight);
	int minZ = std::max(coords[2] - 5, 0);
	int maxZ = std::min(coords[2] + 6, 2 * halfLength);

	for (int x = minX ; x < maxX ; x++) {
		for (int y = minY; y < maxY; y++) {
			for (int z = minZ; z < maxZ ; z++)
			{
				if (map[x][y][z]->beRendered())
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

void Ground::render(GLuint program, glm::vec3 camPos)
{
	int count = 0;
	std::vector<BlockPtr> transparentBlocks;
	for (int x = 0; x < 2*halfWidth; x++)
	{
		for (int y = 0; y < 2*halfHeight; y++)
		{
			for (int z = 0; z < 2*halfLength; z++)
			{				
				if (map[x][y][z]->beRendered())
				{
					count++;
					if (map[x][y][z]->isTransparent())
						transparentBlocks.push_back(map[x][y][z]);
					else
						map[x][y][z]->render(program);
				}
			}
		}
	}

	std::map<float, BlockPtr> sorted;
	for (BlockPtr block : transparentBlocks)
	{
		float distance = glm::length(camPos - block->getPosition());
		sorted[distance] = block;
	}
	for (std::map<float, BlockPtr>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		it->second->render(program);
}

void Ground::clearBuffers()
{
	for (int x = 0; x < 2 * halfWidth; x++)
	{
		for (int y = 0; y < 2 * halfHeight; y++)
		{
			for (int z = 0; z < 2 * halfLength; z++)
			{
				map[x][y][z]->freeBuffer();
			}
		}
	}
}

glm::vec3 Ground::toWorldCoordinates(int x, int y, int z)
{
	float posX = x - halfWidth;
	float posY = y - halfHeight;
	float posZ = z - halfLength;
	return glm::vec3(posX, posY, posZ);
}

std::vector<int> Ground::toMapCoordinates(glm::vec3 worldCoords)
{
	int posX = std::round(worldCoords.x) + halfWidth;
	int posY = std::round(worldCoords.y) + halfHeight;
	int posZ = std::round(worldCoords.z) + halfLength;
	return std::vector<int>{posX, posY, posZ};
}
#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
enum class Type { AIR, SOLID, TRANSPARENT };

class Texture
{
private:
	Type type = Type::AIR;
	std::vector<int> positionsInArray;

public:
	Texture(Type blockType) : type(blockType), positionsInArray(std::vector<int>(0, -1)) {}

	Texture() : positionsInArray(std::vector<int>(0, -1)) {}
	
	void setType(Type type) { this->type = type; }

	void addSample(int posInArray)
	{
		positionsInArray.push_back(posInArray);
	}

	int getLocationInArray(int index)
	{
		int size = positionsInArray.size();
		if (size == 0)
			return 0;

		else if (size == 1)
			return positionsInArray[0];

		return positionsInArray[index];
	}

	Type getType() { return type; }
};
#endif // !TEXTURE_H
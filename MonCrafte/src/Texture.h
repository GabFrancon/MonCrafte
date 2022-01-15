#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"
enum class Type { AIR, SOLID, TRANSPARENT };

class Texture
{
private:
	Type type = Type::AIR;
	std::vector<int> positionsInArray;
	std::vector<GLuint> texID;

public:
	Texture() : texID(std::vector<GLuint>(0, 0)), positionsInArray(std::vector<int>(0, -1)) {}
	
	void setType(Type type) { this->type = type; }

	void addSample(GLuint texture, int posInArray)
	{
		texID.push_back(texture); 
		positionsInArray.push_back(posInArray);
	}

	GLuint getTexID(int index)
	{
		if (index < texID.size())
			return texID[index];
		return texID[0];
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
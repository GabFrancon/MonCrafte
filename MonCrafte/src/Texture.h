#ifndef TEXTURE_H
#define TEXTURE_H

#include <algorithm>
#include <vector>
#include <memory>
#include <cmath>
#include <map>
#include <random>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Texture
{
private:
	std::vector<int> positionsInArray;
	std::vector<GLuint> texID;

public:
	Texture() : texID(std::vector<GLuint>(0, 0)), positionsInArray(std::vector<int>(0, -1)) {}

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
		if(index<positionsInArray.size())
			return positionsInArray[index];
		return positionsInArray[0];
	}
};
#endif // !TEXTURE_H
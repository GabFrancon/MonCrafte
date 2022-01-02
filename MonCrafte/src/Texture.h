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
	GLuint texID = 0;
	int locationInArray = -1;

public:
	Texture() {}

	void setTexID(GLuint texture) { texID = texture; }
	void setLocationInArray(int pos) { locationInArray = pos; }

	GLuint getTexID() { return texID; }
	int getLocationInArray() { return locationInArray; }
};
#endif // !TEXTURE_H


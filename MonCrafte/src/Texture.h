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
	std::map<std::string, GLuint> collection;
	int locationInArray = -1;

public:
	Texture() {}
	Texture(GLuint texture)
	{
		collection["side"] = texture;
		collection["top"] = texture;
		collection["bottom"] = texture;
	}
	Texture(GLuint top, GLuint bottom, GLuint side)
	{
		add("top", top);
		add("bottom", bottom);
		add("side", side);
	}
	void add(std::string name, GLuint texture) { collection[name] = texture; }
	void setLocationInArray(int pos) { locationInArray = pos; }
	GLuint get(std::string name) { return collection[name]; }
	int getLocationInArray() { return locationInArray; }
};
#endif // !TEXTURE_H


#ifndef GEOMETRY_H
#define GEOMETRY_H

#define _USE_MATH_DEFINES

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

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Geometry
{
protected:
	std::vector<float> vertexPositions;
	std::vector<float> vertexNormals;
	std::vector<float> vertexTextures;

	GLuint vao = 0;
	GLuint posVbo = 0;
	GLuint norVbo = 0;
	GLuint texVbo = 0;

public:
	Geometry() {};
	virtual void bindToGPU() = 0;
	virtual void drawGeometry() = 0;

	void freeBuffer()
	{
		/*glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &posVbo);
		glDeleteBuffers(1, &norVbo);
		glDeleteBuffers(1, &texVbo);*/
	}
};

typedef std::shared_ptr<Geometry> GeometryPtr;

#endif // !GEOMETRY_H